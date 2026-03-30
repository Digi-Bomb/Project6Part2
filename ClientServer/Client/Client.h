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

	std::string clientID;
    FileReader* fileReader;

public:
    Client(const char* ip, int port, const char* fileName);
    ~Client();
    const char* getServerIP() const;
    int getServerPort() const;
    void setServerIP(const char* ip);
    void setServerPort(int port);

    const std::string getClientID() const;
    static std::string generateClientID();

    void run();
    bool sendStartOfFile();
    bool sendTelemetry(const std::string& data);
    bool sendEndOfFile();
};