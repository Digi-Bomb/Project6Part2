#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "../Shared/Packet.h"
#include <iostream>
#include <map>
#include <ctime>
#include "ClientRecord.h"
#include "DataLogging.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <shared_mutex>

class Server {
private: 
    SOCKET serverSocket;
    std::map<std::string, std::time_t> activeClients; // Client and last received transmission
    std::shared_mutex activeClientsMutex;
    std::map<std::string, ClientRecord> recorder; // Client and their recorder
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    DataLogging dataLoggr;

public:
    //Server()
    ~Server();
    void beginServerConnections();
    void validateConnections();
    void receiveConnections(char* buffer, sockaddr_in clientAddr, int bytesReceived);
    void updateActiveClient(std::string clientID, time_t lastReceivedPacket);
    void addRecorderToClient(std::string clientID, std::string planeFileName, time_t connectionTime);
    
    ClientRecord getClientsRecorder(std::string clientID);
    void callDataLogic(std::string clientID, float fuel, time_t timeReceived);
    time_t convertStringToTime(std::string parsedTime, std::string parsedDate);
    void logFinalData(std::string clientID);
    //char* clientID, char* planeFileName, time_t lastSeen, float initFuel
};