#include <iostream>
#include <fstream>
#include "DataLogging.h"

/**
* @file DataLogging.cpp
* @brief The DataLogging c++ logic. Logs datetime data, ongoing fuel levels, fuel consumption, per client ID (logs client ID as well)
*/

DataLogging::~DataLogging() {}


void DataLogging::DataLogic(char *path)
{
}

/**
    * @brief initConnectionLog is called only once by the server, and opens a log file to track client connections
*/
void DataLogging::initConnectionLog() {
    this->connectionLog.open("connection.log", std::ios::app);

    if (!this->connectionLog.is_open()) {
        std::cerr << "Error opening connection.log\n";
    }
}

/**
    * @brief logConnection initializes each client's connection, and is meant to audit who is connection.
    * @param [string] clientID, as it sounds, holds the ID of the client who's clientRecord is being requested
    * @param [float] fuel holds the parsed fuel value from the client's initial packet (= 0.00)
    * @param [float] consumption holds the ongoing fuel consumption average value for the respective client's initial packet (= 0.00)
*/
void DataLogging::logConnection(char *clientID, float fuel, float consumption){
    
    this->connLogMutex.lock();
    this->connectionLog << "[CONNECT] Client: " << clientID
         << " | Fuel: " << fuel
         << " | Average Consumption: " << consumption << "\n";
    this->connLogMutex.unlock();
    //this->connectionLog.close();
}

/**
    * @brief populateDataLogMapper creates a unique ofstream variable for each unique filename received, keeping the file opened open until there are no active client connections
    *
      NOTE: This function ensures that only ONE file is opened for logging PER FILENAME
    * @param [string] planeFileName, as it sounds, holds the name of the flight who's data is being transmitted
* 
*/
void DataLogging::populateDataLogMapper(std::string planeFileName) {
    std::cout << "called" << std::endl;
    std::lock_guard<std::mutex> lock(this->LogMutexMapMutex);

    // create mutex for this file (once)
    this->LogMutexes.try_emplace(planeFileName);

    // create file stream ONLY if it doesn't exist
    if (this->DataLogs.find(planeFileName) == this->DataLogs.end())
    {
        auto stream = std::make_unique<std::ofstream>(
            planeFileName,
            std::ios::app
        );

        if (!stream->is_open()) {
            std::cerr << "Failed to open log file: " << planeFileName << "\n";
            return;
        }

        this->DataLogs.emplace(planeFileName, std::move(stream));
    }
}

/**
    * @brief logData does exactly what it sounds like, logging the incoming telemetry data to each flight's log.
    * @param [char*] clientID, as it sounds, holds the ID of the client who's clientRecord is being requested
    * @param [float] fuel holds the parsed fuel value from the respective client's received packet
    * @param [float] consumption holds the ongoing fuel consumption average value for the respective client
    * @param [char*] planeFileName, as it sounds, holds the name of the flight who's data is being transmitted
*/
void DataLogging::logData(char *clientID, float fuel, float consumption, char *planeFileName){
 
    std::mutex& mtx = this->LogMutexes.at(planeFileName);
    std::lock_guard<std::mutex> lock(mtx);

    auto it = this->DataLogs.find(planeFileName);
    if (it == this->DataLogs.end()) {
        return; //No ofstream file open found
    }

    auto& file = *it->second;

    file << "[DATA] Client: " << clientID
         << " | Fuel: " << fuel
         << " | Consumption: " << consumption << "\n";

}

/**
    * @brief logEOF logs the passed client's final fuel consumption average across the duration of their flight
    * @param [string] clientID, as it sounds, holds the ID of the client who's clientRecord is being requested
    * @param [float] consumption holds the final fuel consumption average value for the respective client
    * @param [char*] planeFileName, as it sounds, holds the name of the flight who's data is being transmitted
*/
void DataLogging::logEOF(std::string clientID, float consumption, std::string planeFileName){
    std::mutex& mtx = this->LogMutexes.at(planeFileName);
    std::lock_guard<std::mutex> lock(mtx);

    auto it = this->DataLogs.find(planeFileName);
    if (it == this->DataLogs.end()) {
        return; //No ofstream file open found
    }

    auto& file = *it->second;

    file << "[EOF] Client: " << clientID
        << " | Final Consumption: " << consumption << "\n";
}

/**
    * @brief closeAllFiles, aptly named, closes any open ofstream variables/files. This essentially finalizes the logging of a flight's data.
    *
    NOTE: This is called ONLY when there are NO ACTIVE CLIENTS.
*/
void DataLogging::closeAllFiles()
{
    std::lock_guard<std::mutex> lock(LogMutexMapMutex);

    for (auto& entry : DataLogs)
    {
        auto& key = entry.first;
        auto& filePtr = entry.second;

        if (filePtr && filePtr->is_open())
        {
            filePtr->flush();
            filePtr->close();
        }
    }

    DataLogs.clear();
    LogMutexes.clear(); 
    std::cout << "All files closed" << std::endl;
}

/**
    * @brief logError, aptly named, ensures that an audit is created if any errors occur during attempting to log some data.
    * @param [char*] clientID, as it sounds, holds the ID of the client who's clientRecord is being requested
    * @param [char*] error holds the specifics of the error thrown by the system/server
*/
void DataLogging::logError(char *clientID, char *error){
    std::ofstream file("error.log", std::ios::app);

    if (!file) {
        std::cerr << "Error opening error.log\n";
        return;
    }

    file << "[ERROR] Client: " << clientID
         << " | Message: " << error << std::endl;

    file.close();
}