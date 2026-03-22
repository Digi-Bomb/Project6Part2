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
    char* serverIP;
    int serverPort;

    char clientID[10];
    FileReader* fileReader;

public:
    Client(const char* ip, int port, const char* fileName, const char* id);
    ~Client();
    const char* getClientID() const;
    const char* getServerIP() const;
    int getServerPort() const;
    int getFileLineNumber() const;
    void setClientID(const char* id);
    void setServerIP(const char* ip);
    void setServerPort(int port);
    void run();
    bool sendStartOfFile();
    bool sendTelemetry(const std::string& data);
    bool sendEndOfFile();
};