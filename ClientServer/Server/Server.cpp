#include <windows.networking.sockets.h>
#include <iostream>
#include <string>
#include "Server.h"
#include <vector>
#include <sstream>

// Static values for pointer arithmetic
#define HEADER_SIZE 16
#define TAIL_SIZE 4

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    Server ser = Server();
    //TO DO: Initiate background processes here (checking each minute for last received message from each Client)
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

    //Create a local buffer variable for connection receival. Additionally track the length of the client address
    char buffer[1024];
    int clientLen = sizeof(this->clientAddr);

   
    // Ongoing Receive loop, infinite at the moment
    while (true) {

        // Receive a packet, storing the # of bytes received in a local variable
        int bytesReceived = recvfrom(
            this->serverSocket,
            buffer,
            sizeof(buffer),
            0,
            (sockaddr*)&this->clientAddr,
            &clientLen
        );

        // Handle socket errors
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recvfrom failed\n";
            continue;
        }

        // Make sure we received at least a header
        if (bytesReceived < HEADER_SIZE) {
            std::cerr << "Received packet too small!\n";
            continue;
        }
       
        
        else if (bytesReceived) {
            //Anytime there is a receive, the server needs to handle transmissed data
            receiveConnections(buffer, clientLen, bytesReceived);

            // Print active clients list:
            /*std::cout << "Active clients:\n";
            for (const auto& client : this->activeClients) {
                char timeStr[26];
                ctime_s(timeStr, sizeof(timeStr), &client.second);
                std::cout << client.first << " last seen at: " << timeStr;
            }*/
        }
       
        
    }

    std::cout << "Final clients:\n";
    for (const auto& client : this->activeClients) {
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &client.second);
        std::cout << client.first << " last seen at: " << timeStr;
    }
    // Cleanup (not yet reachable, but good practice)
    closesocket(this->serverSocket);
    WSACleanup();
}

// Function that handles each received packet
// SOF: Adds client connection information to ActiveClients Mapper (local connection time and ID)
// EOF: Removes Client information from ActiveClients Mapper at the Client ID (No longer considered an Active Client)
// TELEMETRY PACKET: Deserializes packet and parses data (into date, time, and fuel level). Additionally sends this information to the Data Logic Module (along with the Client ID this is associated with)
void Server::receiveConnections(char* buffer, int clientLength, int bytesReceived) {

    Packet* cur = new Packet(buffer);;
    std::string clientID(cur->getClientID());
    time_t timeNow;

    if (cur->getStartFlag()) {

        std::cout << "Received Client: " << clientID << std::endl;
       
        
        time(&timeNow);

        updateActiveClient(clientID, timeNow); //Set mapping table entry to client ID

    }

    if (cur->getEndFlag()) {
        std::cout << "This is an end of flight packet: " << std::endl;

        this->activeClients.erase(clientID);

        //TODO: Remove mapping table entry
    }

    //TO DO: handle and parse body data:
    else if(!cur->getEndFlag() && !cur->getStartFlag() ){
        
        time(&timeNow);
        updateActiveClient(clientID, timeNow);
        // Initialize variables for local storage
        std::vector<std::string> bodyParts;
        std::string part, date, time, fuel;

        // Determine the size of the received data, and obtain the data seperately.
        int CurBodySize = bytesReceived - HEADER_SIZE - TAIL_SIZE;
        char* curBodyData = cur->getTelemetryData();

        std::string bodyData(curBodyData, CurBodySize);
        std::stringstream ss(bodyData);

        // Iterate through the received data, parsing by ',' delimeters. Add each subsection to the local Vector
        while (std::getline(ss, part, ',')) {
            bodyParts.push_back(part);
        }
        
        // Determine the position within the string of the first space (because the date in these files is not comma-seperated from the time
        size_t spacePos = bodyParts[0].find(" ");

        // Only if a space exists, parse the data
        if (spacePos != std::string::npos) {
            date = bodyParts[0].substr(0, spacePos);
            time = bodyParts[0].substr(spacePos + 1);
        }

        else {
            date = bodyParts[0];
            time = "";
        }

        // Store the fuel value
        fuel = bodyParts[1];


        // Debugging print
        std::cout << "date: " << date << ", time: " << time << ", fuel: " << fuel << std::endl;


        // TO DO: Call Data Logic Module!!!!!
       
    }

}
void Server::updateActiveClient(std::string clientID, time_t lastReceivedPacket) {
    
    // Add the currect client id to activeClients
    this->activeClients[clientID] = lastReceivedPacket;

}

