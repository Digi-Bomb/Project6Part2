// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <windows.networking.sockets.h>
#include <iostream>
#include <string>
#include "Server.h"

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    Server ser = Server();
    ser.beginServerConnections();
    std::cout << "Hello World!\n";
}

void Server::beginServerConnections() {
    WSADATA wsaData;

    // 1. Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return;
    }

    // 2. Create UDP socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    // 3. Set up server address
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(67670);
    this->serverAddr.sin_addr.s_addr = INADDR_ANY; // accept from any client

    // 4. Bind socket
    if (bind(this->serverSocket, (sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(this->serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "UDP Server listening on port 67670...\n";

    // 5. Receive loop
    char buffer[1024];
    int clientLen = sizeof(this->clientAddr);

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

        buffer[bytesReceived] = '\0'; // null terminate

        std::cout << "Received: " << buffer << std::endl;

        // // send response back
        // std::string reply = "Message received";
        // sendto(
        //     serverSocket,
        //     reply.c_str(),
        //     reply.size(),
        //     0,
        //     (sockaddr*)&clientAddr,
        //     clientLen
        // );
    }

    // Cleanup (never reached in this loop, but good practice)
    closesocket(serverSocket);
    WSACleanup();
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
