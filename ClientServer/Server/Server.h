#pragma once
#include <windows.networking.sockets.h>
#include <string>
#include "Packet.h"
//#include "FileReader.h"
#include <iostream>
#include <map>

// using namespace std;

class Server {
private: 
    SOCKET serverSocket;
    std::map<std::string, std::string> activeClients;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;

   // char*

public:
    //Server()
    ~Server();
    void beginServerConnections();
    
};