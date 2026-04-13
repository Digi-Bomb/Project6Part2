#pragma once

#ifdef _WIN32
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <string>
#include "../Shared/Packet.h"
#include "FileReader.h"

class Client {
private:
#ifdef _WIN32
    SOCKET clientSocket;
#else
    int clientSocket;
#endif
    sockaddr_in serverAddr;
    char* serverIP;
    int serverPort;

    char clientID[37];
    FileReader* fileReader;

public:
    Client(const char* ip, int port, const char* fileName, const char* id);
    ~Client();
    const char* getClientID() const;
    const char* getServerIP() const;
    int getServerPort() const;
    void setClientID(const char* id);
    void setServerIP(const char* ip);
    void setServerPort(int port);
    void run();
    bool sendStartOfFile();
    bool sendTelemetry(const std::string& data);
    bool sendEndOfFile();
};