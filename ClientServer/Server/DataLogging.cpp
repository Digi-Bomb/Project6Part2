#include <iostream>
#include <fstream>
#include "DataLogging.h"

DataLogging::~DataLogging() {}

void DataLogging::DataLogic(char *path)
{
}

void DataLogging::logConnection(char *clientID, float fuel, float consumption){
    std::ofstream file("connection.log", std::ios::app);

    if (!file) {
        std::cerr << "Error opening connection.log\n";
        return;
    }

    file << "[CONNECT] Client: " << clientID
         << " | Fuel: " << fuel
         << " | Average Consumption: " << consumption << std::endl;

    file.close();
}

void DataLogging::logData(char *clientID, float fuel, float consumption, char *planeFileName){
    std::ofstream file(planeFileName, std::ios::app);

    if (!file) {
        std::cerr << "Error opening file: " << planeFileName << std::endl;
        return;
    }

    file << "[DATA] Client: " << clientID
         << " | Fuel: " << fuel
         << " | Consumption: " << consumption << std::endl;

    file.close();
}

void DataLogging::logEOF(std::string clientID, float consumption, std::string planeFileName){
    std::ofstream file(planeFileName, std::ios::app);

    if (!file) {
        std::cerr << "Error opening file: " << planeFileName << std::endl;
        return;
    }

    file << "[EOF] Client: " << clientID
         << " | Final Consumption: " << consumption << std::endl;

    file.close();
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