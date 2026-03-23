#include <windows.networking.sockets.h>
#include <iostream>
#include <string>
#include "Server.h"

#define HEADER_SIZE 16

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    Server ser = Server();
    ser.beginServerConnections();
}

Server::~Server() {}

void Server::beginServerConnections() {
    WSADATA wsaData;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return;
    }

    // Create UDP socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    // Set up server address
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(6767);
    this->serverAddr.sin_addr.s_addr = INADDR_ANY; // accept from any client

    // 4. Bind socket
    if (bind(this->serverSocket, (sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(this->serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "UDP Server listening on port 6767...\n";

    char buffer[1024];
    int clientLen = sizeof(this->clientAddr);

    // Receive loop
    // Need to change into a seperate "Receive connections" function
    while (true) {
        int bytesReceived = recvfrom(
            this->serverSocket,
            buffer,
            sizeof(buffer),
            0,
            (sockaddr*)&this->clientAddr,
            &clientLen
        );

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recvfrom failed\n";
            continue;
        }

        // Make sure we received at least a header
        if (bytesReceived < HEADER_SIZE) {
            std::cerr << "Received packet too small!\n";
            continue;
        }

      
        Packet* cur = new Packet(buffer);;

        if (cur->getStartFlag()) {

            std::cout << "This is a start of flight packet: " << std::endl;
            std::string clientID(cur->getClientID());
            time_t timeNow;
            time(&timeNow);

            setActiveClient(clientID, timeNow); //Set mapping table entry to client ID
            
        }

        if (cur->getEndFlag()) {
            std::cout << "This is an end of flight packet: " << std::endl;
            //TODO: Remove mapping table entry
        }

        //TO DO: handle and parse body data:
        // else(){
        // 
        // }
        
        // print active clients
        std::cout << "Active clients:\n";
        for (const auto& client : this->activeClients) {
            char timeStr[26];
            ctime_s(timeStr, sizeof(timeStr), &client.second);
            std::cout << client.first << " last seen at: " << timeStr;
        }
      
    }

    // Cleanup (not yet reachable, but good practice)
    closesocket(this->serverSocket);
    WSACleanup();
}

void Server::setActiveClient(std::string clientID, time_t lastReceivedPacket) {
    
    this->activeClients[clientID] = lastReceivedPacket;
    //std::cout << "New client added: " << clientID << std::endl;

}

