#pragma once
#include <iostream>

class DataLogging
{
public:
    char *logPath;

    ~DataLogging();
    void DataLogic(char* path);
    void logConnection(char* clientID, float weight, float avg);
    void logData(char* clientID, float weight, float avg, char* planeFileName);
    void logEOF(std::string, float avg, std::string planeFileName);
    void logError(char* clientID, char* error);
    
};