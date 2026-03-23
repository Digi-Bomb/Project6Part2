#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <windows.networking.sockets.h>
#include <string>
#include "Packet.h"
#include <iostream>
#include <map>
#include <ctime>


class Server {
private: 
    SOCKET serverSocket;
    std::map<std::string, std::time_t> activeClients;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;

public:
    //Server()
    ~Server();
    void beginServerConnections();
    void receiveConnections(char* buffer, int clientLength);
    void setActiveClient(std::string clientID, time_t lastReceivedPacket);
    
};