#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <mutex>

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