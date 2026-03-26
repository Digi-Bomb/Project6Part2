#pragma once
#include <string>
#include <iostream>

class ClientRecord
{
public:
    char *clientID;
    char *planeFileName;
    time_t lastSeen;
    float currentAverageFuel;
    int fuelSumCounter;

    ~ClientRecord();
    ClientRecord(char* clientID, char* planeFileName, time_t lastSeen);
    bool updateAverageFuel(double fuel);
    void updateFuelSumCounter();
    float getAverageFuel();
    int getFuelSumCounter();
};
