#ifdef _WIN32
#define NOMINMAX  // Prevent Windows headers from defining min/max macros
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h> // for Sleep
#pragma comment(lib, "Ws2_32.lib")
#include <direct.h>
#undef small  // Windows headers define 'small' as a macro which conflicts with Boost
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#endif

#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "../Shared/Packet.h"
#include "Client.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

char* generateID()
{
    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();
    std::string uuidStr = boost::uuids::to_string(id);

    char* IDchar = new char[37];
#ifdef _WIN32
    strncpy_s(IDchar, 37, uuidStr.c_str(), 36);
#else
    strncpy(IDchar, uuidStr.c_str(), 36);
#endif
    IDchar[36] = '\0';
    return IDchar;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <telemetry_file>" << std::endl;
        return 1;
    }

    char* clientID = generateID();
    Client cli = Client(argv[1], atoi(argv[2]), argv[3], clientID);
    cli.run();
    delete[] clientID;

    return 0;
    // clean up like calling destructors for Client and its fileReader are done here (including closing socket and handling file i/o stuff)
}

Client::Client(const char* ip, int port, const char* fileName, const char* id) {
    this->serverPort = port;
    this->serverIP = strdup(ip);

#ifdef _WIN32
    strncpy_s(this->clientID, sizeof(this->clientID), id, 36);
#else
    strncpy(this->clientID, id, 36);
#endif
    this->clientID[36] = '\0';

    this->fileReader = new FileReader(fileName);
    if (!this->fileReader->openFile()) {
        std::cerr << "Error: Could not open telemetry file " << fileName << std::endl;
        std::cerr << "Trying to open: " << fileName << std::endl;
    }

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }
#endif

    this->clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
    if (this->clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }
#else
    if (this->clientSocket < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return;
    }
#endif

    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(this->serverPort);

    if (inet_pton(AF_INET, this->serverIP, &this->serverAddr.sin_addr) != 1) {
        std::cerr << "Invalid server IP address: " << this->serverIP << std::endl;
    }
}

Client::~Client()
{
    if (this->fileReader != nullptr) {
        delete this->fileReader;
        this->fileReader = nullptr;
    }

    if (this->serverIP != nullptr) {
        free(this->serverIP);
        this->serverIP = nullptr;
    }

#ifdef _WIN32
    if (this->clientSocket != INVALID_SOCKET) {
        closesocket(this->clientSocket);
    }
    WSACleanup();
#else
    if (this->clientSocket >= 0) {
        close(this->clientSocket);
    }
#endif
}

const char* Client::getClientID() const {
    return this->clientID;
}

const char* Client::getServerIP() const {
    return this->serverIP;
}

int Client::getServerPort() const {
    return this->serverPort;
}

void Client::setClientID(const char* id) {
    if (id) {
#ifdef _WIN32
        strncpy_s(this->clientID, sizeof(this->clientID), id, 36);
#else
        strncpy(this->clientID, id, 36);
#endif
        this->clientID[36] = '\0';
    }
}

void Client::setServerIP(const char* ip) {
    if (ip) {
        if (this->serverIP) free(this->serverIP);

        this->serverIP = strdup(ip);

        if (inet_pton(AF_INET, this->serverIP, &this->serverAddr.sin_addr) != 1) {
            std::cerr << "Invalid server IP address: " << this->serverIP << std::endl;
        }
    }
}

void Client::setServerPort(int port) {
    this->serverPort = port;
    this->serverAddr.sin_port = htons(this->serverPort);
}

void Client::run()
{
    std::string skipPrefix = "FUEL TOTAL QUANTITY,";
    if (!this->fileReader->openFile())
    {
        std::cerr << "Unable to open file" << std::endl;
    }

    this->sendStartOfFile();

    std::string line;

    while (this->fileReader->readLine(line))
    {
        if (line.find(skipPrefix) == 0) {
            line = line.substr(skipPrefix.length());
        }

        this->sendTelemetry(line);

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    this->sendEndOfFile();
}

bool Client::sendStartOfFile()
{
    Packet pkt;
    pkt.setClientID(this->clientID);
    pkt.setStartFlag(true);
    pkt.setEndFlag(false);

    std::string info = (this->fileReader->getFilePath());
    pkt.setData((char*)info.c_str(), (int)info.length());

    int totalSize = 0;
    char* buffer = pkt.serialize(totalSize);

    std::vector<char> safeBuffer(buffer, buffer + totalSize);

    int bytesSent = sendto(this->clientSocket, safeBuffer.data(), totalSize, 0,
        (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));

    return (bytesSent >= 0);
}

bool Client::sendTelemetry(const std::string& data)
{
    Packet pkt;
    pkt.setClientID(this->clientID);
    pkt.setStartFlag(false);
    pkt.setEndFlag(false);

    pkt.setData((char*)data.c_str(), (int)data.length());

    int totalSize = 0;
    char* buffer = pkt.serialize(totalSize);

    std::vector<char> safeBuffer(buffer, buffer + totalSize);

    int bytesSent = sendto(this->clientSocket, safeBuffer.data(), totalSize, 0,
        (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));

    if (bytesSent < 0) {
#ifdef _WIN32
        std::cerr << "Telemetry failed to send: " << WSAGetLastError() << std::endl;
#else
        std::cerr << "Telemetry failed to send: " << strerror(errno) << std::endl;
#endif
        return false;
    }

    return true;
}

bool Client::sendEndOfFile()
{
    Packet pkt;
    pkt.setClientID(this->clientID);
    pkt.setStartFlag(false);
    pkt.setEndFlag(true);

    pkt.setData((char*)"", 0);

    int totalSize = 0;
    char* buffer = pkt.serialize(totalSize);

    std::vector<char> safeBuffer(buffer, buffer + totalSize);

    int bytesSent = sendto(this->clientSocket, safeBuffer.data(), totalSize, 0,
        (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));

    return (bytesSent >= 0);
}