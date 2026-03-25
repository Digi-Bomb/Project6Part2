#include <iostream>
#include <fstream>
#include "DataLogging.h"

DataLogging::~DataLogging() {}

void DataLogic(char *path)
{
}

void logConnection(char *clientID, float weight, float avg){
    std::ofstream file("connection.log", std::ios::app);

    if (!file) {
        std::cerr << "Error opening connection.log\n";
        return;
    }

    file << "[CONNECT] Client: " << clientID
         << " | Weight: " << weight
         << " | Avg: " << avg << std::endl;

    file.close();
}

void logData(char *clientID, float weight, float avg, char *planeFileName){
    std::ofstream file(planeFileName, std::ios::app);

    if (!file) {
        std::cerr << "Error opening file: " << planeFileName << std::endl;
        return;
    }

    file << "[DATA] Client: " << clientID
         << " | Weight: " << weight
         << " | Avg: " << avg << std::endl;

    file.close();
}

void logEOF(char *clientID, float avg, char *planeFileName){
    std::ofstream file(planeFileName, std::ios::app);

    if (!file) {
        std::cerr << "Error opening file: " << planeFileName << std::endl;
        return;
    }

    file << "[EOF] Client: " << clientID
         << " | Final Avg: " << avg << std::endl;

    file.close();
}

void logError(char *clientID, char *error){
    std::ofstream file("error.log", std::ios::app);

    if (!file) {
        std::cerr << "Error opening error.log\n";
        return;
    }

    file << "[ERROR] Client: " << clientID
         << " | Message: " << error << std::endl;

    file.close();
}