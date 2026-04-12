#include <iostream>
#include <fstream>
#include "DataLogging.h"

DataLogging::~DataLogging() {}

void DataLogging::DataLogic(char *path)
{
}
void DataLogging::initConnectionLog() {
    this->connectionLog.open("connection.log", std::ios::app);

    if (!this->connectionLog.is_open()) {
        std::cerr << "Error opening connection.log\n";
    }
}

void DataLogging::logConnection(char *clientID, float fuel, float consumption){
    
    this->connLogMutex.lock();
    this->connectionLog << "[CONNECT] Client: " << clientID
         << " | Fuel: " << fuel
         << " | Average Consumption: " << consumption << "\n";
    this->connLogMutex.unlock();
    //this->connectionLog.close();
}

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

void DataLogging::logData(char *clientID, float fuel, float consumption, char *planeFileName){
   /* std::ofstream file(planeFileName, std::ios::app);

    if (!file) {
        std::cerr << "Error opening file: " << planeFileName << std::endl;
        return;
    }*/
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

    //this->LogMutexes[planeFileName].unlock();
    //file.close();
}

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

   // file.close();
}

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