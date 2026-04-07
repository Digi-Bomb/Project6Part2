#pragma comment(lib, "Ws2_32.lib")

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>
#include <ctime>
#include <stdexcept>

#include "Server.h"

boost::asio::thread_pool serverPool(30);

// Protect recorder map access across worker threads
static std::mutex recorderMutex;

int main()
{
    Server ser;

    // TO DO: Initiate background processes here
    std::thread backgroundConnectionCleaner(&Server::validateConnections, &ser);

    //std::thread poolThread([]() {
    //    serverPool.join();  // runs the worker threads
    //    });

    ser.beginServerConnections();

    backgroundConnectionCleaner.join();
    serverPool.join();

    return 0;
}

Server::~Server() {}

void Server::beginServerConnections() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return;
    }

    this->serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(6767);
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->serverSocket, (sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(this->serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "UDP Server listening on port 6767...\n";

    char buffer[1024];

    while (true) {
        sockaddr_in clientAddrLocal;   
        int clientLen = sizeof(clientAddrLocal);

        int bytesReceived = recvfrom(
            this->serverSocket,
            buffer,
            sizeof(buffer),
            0,
            (sockaddr*)&clientAddrLocal,   
            &clientLen
        );

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recvfrom failed\n";
            continue;
        }

        if (bytesReceived < static_cast<int>(Packet::getHeaderSize() + Packet::getTailSize())) {
            std::cerr << "Received packet too small!\n";
            continue;
        }

        if (bytesReceived > 0) {
            auto bufferCopy = std::make_shared<std::vector<char>>(buffer, buffer + bytesReceived);

            
            boost::asio::post(serverPool,
                [this, bufferCopy, clientAddrLocal, bytesReceived]() {

                    this->receiveConnections(
                        bufferCopy->data(),
                        clientAddrLocal,  
                        bytesReceived
                    );
                }
            );
        }
    }

    closesocket(this->serverSocket);
    WSACleanup();
}
// Function that handles each received packet
void Server::receiveConnections(char* buffer, sockaddr_in clientAddr, int bytesReceived){
    (void)clientAddr; // unused right now

    try {
        Packet cur(buffer, bytesReceived);
        std::string clientID(cur.getClientID());
        time_t timeNow{};

        if (cur.getStartFlag()) {
            std::cout << "Received Client: " << clientID << std::endl;
            std::string flightFile(cur.getTelemetryData());

            time(&timeNow);

            updateActiveClient(clientID, timeNow);
            addRecorderToClient(clientID, flightFile, timeNow);

            float initialFuel = 0.0f;
            this->dataLoggr.logConnection(
                (char*)clientID.c_str(),
                initialFuel,
                0.0f
            );
        }
        else if (cur.getEndFlag()) {
            std::cout << "This is an end of flight packet: " << clientID << std::endl;

            logFinalData(clientID);

            std::unique_lock<std::shared_mutex> lock(activeClientsMutex);
            this->activeClients.erase(clientID);
        }
        else {
            time(&timeNow);
            updateActiveClient(clientID, timeNow);

            std::vector<std::string> bodyParts;
            std::string part;
            std::string unconverted_date;
            std::string unconverted_time;
            std::string unconverted_fuel;

            // Use packet's own parsed body size, not hardcoded math
            std::string bodyData(cur.getTelemetryData(), cur.getPacketSize());
            std::stringstream ss(bodyData);

            while (std::getline(ss, part, ',')) {
                bodyParts.push_back(part);
            }

            if (bodyParts.size() < 2) {
                std::cerr << "Malformed telemetry packet from client "
                    << clientID << " (fields=" << bodyParts.size() << ")\n";
                return;
            }

            size_t spacePos = bodyParts[0].find(' ');

            if (spacePos != std::string::npos) {
                unconverted_date = bodyParts[0].substr(0, spacePos);
                unconverted_time = bodyParts[0].substr(spacePos + 1);
            }
            else {
                unconverted_date = bodyParts[0];
                unconverted_time = "";
            }

            unconverted_fuel = bodyParts[1];

            float fuel = 0.0f;
            try {
                fuel = std::stof(unconverted_fuel);
            }
            catch (const std::exception&) {
                std::cerr << "Invalid fuel value from client " << clientID
                    << ": " << unconverted_fuel << '\n';
                return;
            }

            time_t parsedTime = convertStringToTime(unconverted_time, unconverted_date);
            callDataLogic(clientID, fuel, parsedTime);
        }
    }
    catch (const std::out_of_range& e) {
        std::cerr << "Out-of-range while processing packet: " << e.what() << '\n';
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while processing packet: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Unknown exception while processing packet.\n";
    }
}

ClientRecord Server::getClientsRecorder(std::string clientID) {
    std::lock_guard<std::mutex> lock(recorderMutex);

    auto it = this->recorder.find(clientID);
    if (it == this->recorder.end()) {
        throw std::out_of_range("Client recorder not found for clientID: " + clientID);
    }

    return it->second;
}

void Server::callDataLogic(std::string clientID, float fuel, time_t timeReceived) {
    std::lock_guard<std::mutex> lock(recorderMutex);

    auto it = this->recorder.find(clientID);
    if (it == this->recorder.end()) {
        std::cerr << "Recorder entry missing for client " << clientID << '\n';
        return;
    }

    ClientRecord& clientsRecord = it->second;

    clientsRecord.updateFuelConsumption(fuel);
    clientsRecord.setTimeLastSeen(timeReceived);

    this->dataLoggr.logData(
        (char*)clientID.c_str(),
        fuel,
        clientsRecord.getCurrentConsumption(),
        (char*)clientsRecord.getFlightName().c_str()
    );
}

void Server::addRecorderToClient(std::string clientID, std::string planeFileName, time_t connectionTime) {
    std::lock_guard<std::mutex> lock(recorderMutex);

    auto it = this->recorder.find(clientID);
    if (it == this->recorder.end()) {
        this->recorder.emplace(clientID, ClientRecord(clientID, planeFileName, connectionTime));
    }
    else {
        // Refresh an existing record if a duplicate SOF arrives
        it->second.setPlaneFlightName(planeFileName);
        it->second.setTimeLastSeen(connectionTime);
    }
}

void Server::updateActiveClient(std::string clientID, time_t lastReceivedPacket) {
    std::unique_lock<std::shared_mutex> lock(activeClientsMutex);
    this->activeClients[clientID] = lastReceivedPacket;
}

time_t Server::convertStringToTime(std::string parsedTime, std::string parsedDate) {
    std::tm timeStruct = {};

    int month = 0, day = 0, year = 0;
    char sep1 = '\0', sep2 = '\0';
    std::istringstream dateStream(parsedDate);
    dateStream >> month >> sep1 >> day >> sep2 >> year;

    int hour = 0, min = 0, sec = 0;
    char c1 = '\0', c2 = '\0';
    std::istringstream timeStream(parsedTime);

    if (!parsedTime.empty()) {
        timeStream >> hour >> c1 >> min >> c2 >> sec;
    }

    timeStruct.tm_year = year - 1900;
    timeStruct.tm_mon = month - 1;
    timeStruct.tm_mday = day;

    timeStruct.tm_hour = hour;
    timeStruct.tm_min = min;
    timeStruct.tm_sec = sec;

    return mktime(&timeStruct);
}

void Server::logFinalData(std::string clientID) {
    std::lock_guard<std::mutex> lock(recorderMutex);

    auto it = this->recorder.find(clientID);
    if (it == this->recorder.end()) {
        std::cerr << "Cannot log EOF, recorder entry missing for client " << clientID << '\n';
        return;
    }

    ClientRecord& clientsRecord = it->second;

    float finalConsumption = clientsRecord.getCurrentConsumption();
    std::string flightName = clientsRecord.getFlightName();

    this->dataLoggr.logEOF(clientID, finalConsumption, flightName);
}

void Server::validateConnections() {
    auto nextRun = std::chrono::steady_clock::now();
    bool test = true;

    while (test) {
        nextRun += std::chrono::minutes(1);
        std::time_t now = std::time(nullptr);

        std::unique_lock<std::shared_mutex> lock(activeClientsMutex);
        for (auto it = activeClients.begin(); it != activeClients.end();) {
            const std::string& client = it->first;
            std::time_t lastSeen = it->second;

            double diff = std::difftime(now, lastSeen);

            std::cout << "last_seen: " << lastSeen << std::endl;
            std::cout << "diff: " << diff << std::endl;
            std::cout << "now: " << now << std::endl;

            if (diff >= 300) {
                it = activeClients.erase(it);
            }
            else {
                ++it;
            }
        }

        lock.unlock();
        std::this_thread::sleep_until(nextRun);
    }
}