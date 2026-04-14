#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <mutex>

/**
*@file DataLogging.h
* @brief The DataLogging class' header. Accepts parsed packet data and logs this to a log file, seperate and unique for each flight name/file received
*/

/**
* @brief DataLogging class. Private variables listed here:

____________________________________________________________________
* @param [map] DataLogs maintains pointers to each unique filename read by the server from packets
* @param [map] LogMutexes maps a flight name (and thereby, a flight's log file) to a mutex
* @param [mutex] Put simply, LogMutexMapMutex provides a mutex to the mutex mapper, since memory contention for a mapper needs to be avoid
* @param [ofstream] connectionLog represents the file stream variable/connection log file 
* @param [mutex] connLogMutex is the mutex that prevents contention to the above connectionLog filestream 
*/
class DataLogging
{
private:
    std::map<std::string, std::unique_ptr<std::ofstream>> DataLogs;
    std::map<std::string, std::mutex> LogMutexes;
    std::mutex LogMutexMapMutex; //yes, this is confusing. lol.
    std::ofstream connectionLog;
    std::mutex connLogMutex;
public:
    char *logPath;

    ~DataLogging();
    void DataLogic(char* path);
    void initConnectionLog();
    void logConnection(char* clientID, float weight, float avg);
    void populateDataLogMapper(std::string planeFileName);
    void logData(char* clientID, float weight, float avg, char* planeFileName);
    void logEOF(std::string, float avg, std::string planeFileName);
    void logError(char* clientID, char* error);
    void closeAllFiles();
    
};