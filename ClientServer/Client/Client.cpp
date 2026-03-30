#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Packet.h"
#include "Client.h"

#include <direct.h>

// TODO: to be implemented by Colin
char* generateId()
{
    return nullptr;
}

int main(int argc, char* argv[])
{
    const char* ip = argv[1];
    int port = std::stoi(argv[2]);
    std::string fullPathStr = "..\\..\\DataFiles\\" + std::string(argv[3]);
    const char* filePath = fullPathStr.c_str();
    const char* id = generateId();
    Client cli = Client(ip, port, filePath, id);

    return 0;
    // clean up like calling destructors for Client and its fileReader are done here (including closing socket and handling file i/o stuff)
}



Client::Client(const char* ip, int port, const char* fileName, const char* id) {
    this->serverPort = port;
    this->serverIP = _strdup(ip);

    strncpy_s(this->clientID, id, 35);
    this->clientID[35] = '\0';


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
        strncpy_s(this->clientID, id, 9);
        this->clientID[9] = '\0';
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
            if (line.find(skipPrefix) == 0) { // only if it’s at the very start
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
        delete[] buffer;
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
