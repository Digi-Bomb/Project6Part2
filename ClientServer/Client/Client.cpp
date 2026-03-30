#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Packet.h"
#include "Client.h"

#include <direct.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


int main(int argc, char* argv[])
{
	if (argc != 4) {
        std::cerr << "Usage: Client.exe <server_ip> <server_port> <file_path>" << std::endl;
        return 1;
    }
    //Client cli = Client("ip", 0, "filename", generateId());   
    
	//initialize command line args, then create client


	Client cli = Client(argv[1], std::atoi(argv[2]), argv[3]);
    cli.run();
   
    return 0;
    // clean up like calling destructors for Client and its fileReader are done here (including closing socket and handling file i/o stuff)
}

Client::Client(const char* ip, int port, const char* fileName) {
    this->serverPort = port;
    this->serverIP = _strdup(ip);

	//clientID is generated
    this->clientID = generateClientID();
   
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

const std::string Client::getClientID() const {
    return this->clientID;
}

std::string Client::generateClientID()
{
    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();
    return boost::uuids::to_string(id);
}


const char* Client::getServerIP() const {
    return this->serverIP;
}

int Client::getServerPort() const {
    return this->serverPort;
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
    pkt.setStartFlag(true);
    pkt.setEndFlag(false);
    pkt.setClientID(this->clientID);

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
    pkt.setStartFlag(false);
    pkt.setEndFlag(false);
    pkt.setClientID(this->clientID);

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
    pkt.setStartFlag(false);
    pkt.setEndFlag(true);
    pkt.setClientID(this->clientID);

    pkt.setData((char*)"", 0);

    int totalSize = 0;
    char* buffer = pkt.serialize(totalSize);

    int bytesSent = sendto(this->clientSocket, buffer, totalSize, 0,
        (sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
    //delete[] buffer;
    return (bytesSent != SOCKET_ERROR);
}
