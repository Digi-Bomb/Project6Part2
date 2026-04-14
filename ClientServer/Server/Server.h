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
#include <vector>
#include <mutex>
#include <unordered_set>
/**
*@file server.h
* @brief The server class' header. Accepts incoming packets, logs active clients, dictates the behavior of incoming data
*/

/**
    *
    * @brief The server Class. Private variables listed here:
    

    ____________________________________________________________________
    * @param [SOCKET] serverSocket is the socket that accepts incoming connections
    * @param [map] activeClients is the mapper that tracks clients and their recent posts/transmissions
    * @param [shared_mutex] activeClientsMutex prevents memory contention for reading and writing to the activeClients map by server's threads
    * @param [map] recorder maps each client to their ClientRecord class
    * @param [sockaddr_in] serverAddr, socket address for server
    * @param [sockaddr_in] clientAddr, socket address of client
    * @param [DataLogging] dataLoggr is the sole class responsible for logging client averages, names, and received information from each packet
    * @param [unordered_set] uniqueFlightFiles tracks the name of each file/flight received by the server, only storing unique names
    * @param [mutex] flightFileMutex prevents each file's instance from being accessed by more than one thread at a time, preventing memory contention
    */
class Server {

private: 
    
    SOCKET serverSocket;
    std::map<std::string, std::time_t> activeClients; // Client and last received transmission
    std::shared_mutex activeClientsMutex;
    std::map<std::string, ClientRecord> recorder; // Client and their recorder
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    DataLogging dataLoggr;
    std::unordered_set<std::string> uniqueFlightFiles;
    std::mutex flightFileMutex;

public:
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
};