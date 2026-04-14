#pragma once
#include <windows.networking.sockets.h>
#include <string>
#include "../Shared/Packet.h"
#include "FileReader.h"

#pragma comment(lib, "Ws2_32.lib")
/**
*@file Client.h
* @brief The Client's header file. Defines functions and variables for each client to use
*/

/**
* @brief The Client Class. Private variables listed here:
* 

____________________________________________________________________

* @param [SOCKET] clientSocket, the socket the client uses to connect to the server.
* @param [sockaddr_in] serverAddr, the server socket's address to connect to.
* @param [char*] serverIP, the ip of the server, specified in the batch file that runs and builds clients.
* @param [int] serverPort, the port of the server, specified in the batch file that runs and builds clients.
* @param [char*] clientID, the uniquely generated ID of the client.
* @param [FileReader*] the FileReader instance that the client will use to read data from each telemetry file.

*/
class Client {
private:
    SOCKET clientSocket;
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