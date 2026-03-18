#pragma once
#include <windows.networking.sockets.h>
#include <string>
#include "Packet.h"
#include "FileReader.h"

#pragma comment(lib, "Ws2_32.lib")

class Client {
private:
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    std::string serverIP;
    int serverPort;

    char clientID[10];
    FileReader* fileReader;

public:
    Client(const char* ip, int port, const char* fileName, const char* id);
    ~Client();
    void run();
    bool sendStartOfFile();
    bool sendTelemetry(const std::string& data);
    bool sendEndOfFile();
};