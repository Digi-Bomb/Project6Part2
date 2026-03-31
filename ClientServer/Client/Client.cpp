#define NOMINMAX  // Prevent Windows headers from defining min/max macros
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Packet.h"
#include "Client.h"

#include <direct.h>

#undef small  // Windows headers define 'small' as a macro which conflicts with Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

char* generateID()
{
    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();
    std::string uuidStr = boost::uuids::to_string(id);

    char* IDchar = new char[37];
    strncpy_s(IDchar, 37, uuidStr.c_str(), 36);
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
    this->serverIP = _strdup(ip);

    strncpy_s(this->clientID, sizeof(this->clientID), id, 36);
    this->clientID[36] = '\0';

    this->fileReader = new FileReader(fileName);
    if (!this->fileReader->openFile()) {
        std::cerr << "Error: Could not open telemetry file " << fileName << std::endl; // TODO: change to a log
        std::cerr << "Trying to open: " << fileName << std::endl;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl; // TODO: change to a log
        return;
    }

    this->clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl; // TODO: change to a log
        WSACleanup();
        return;
    }

    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(this->serverPort);
    this->serverAddr.sin_addr.s_addr = inet_addr(this->serverIP);
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

    if (this->clientSocket != INVALID_SOCKET) {
        closesocket(this->clientSocket);
    }

    WSACleanup();
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
        strncpy_s(this->clientID, sizeof(this->clientID), id, 36);
        this->clientID[36] = '\0';
    }
}

void Client::setServerIP(const char* ip) {
    if (ip) {
        if (this->serverIP) free(this->serverIP);

        this->serverIP = _strdup(ip);
        this->serverAddr.sin_addr.s_addr = inet_addr(this->serverIP);
    }
}

void Client::setServerPort(int port) {
    this->serverPort = port;
    this->serverAddr.sin_port = htons(this->serverPort);
}

void Client::run()
{
    std::string skipPrefix = "FUEL TOTAL QUANTITY,";
    //this->fileReader
    if (!this->fileReader->openFile())
    {
        std::cerr << "Unable to open file" << std::endl; // TODO: change to a log
    }

    //// send SOF is the only tested function, but others should work
    this->sendStartOfFile();

    while (!this->fileReader->isEOF())
    {

        std::string line;

        // read line
        if (this->fileReader->readLine(line))
        {
            if (line.find(skipPrefix) == 0) { // only if it�s at the very start
                line = line.substr(skipPrefix.length());
            }
            // send telemetry data
            this->sendTelemetry(line);
        }
        Sleep(1000); // so that at most 1 telemetry packet is sent every second
    }
    // send EOF
    this->sendEndOfFile();
}

bool Client::sendStartOfFile()
{
    Packet pkt;
    pkt.setClientID(this->clientID);
    pkt.setStartFlag(true);
    pkt.setEndFlag(false);

    std::string info = (this->fileReader->getFilePath());
    //std::cout << "The info being set is: " << info << std::endl;
    pkt.setData((char*)info.c_str(), (int)info.length());

    int totalSize = 0;


    char* buffer = pkt.serialize(totalSize);

    int bytesSent = sendto(this->clientSocket, buffer, totalSize, 0,
        (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
    //delete[] buffer; THIS BUFFER MIGHT BE AN EXTRA DELETION
    //std::cout << "sent SOF" << std::endl;
    return (bytesSent != SOCKET_ERROR);

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

    int bytesSent = sendto(this->clientSocket, buffer, totalSize, 0,
        (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Telemetry failed to send: " << WSAGetLastError() << std::endl; // TODO: change to a log
        return false;
    }
    //delete[] buffer;
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

    int bytesSent = sendto(this->clientSocket, buffer, totalSize, 0,
        (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
    //delete[] buffer;
    return (bytesSent != SOCKET_ERROR);
}
