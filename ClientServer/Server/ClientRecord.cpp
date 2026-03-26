#include <string>
#include <iostream>
#include "ClientRecord.h"

ClientRecord::ClientRecord(char* clientID, char* planeFileName, time_t lastSeen){
    this->clientID = clientID;

    this->planeFileName = planeFileName;

    this->lastSeen = lastSeen;
}

float ClientRecord::getAverageFuel(){
    return currentAverageFuel;
}

int ClientRecord::getFuelSumCounter(){
    return fuelSumCounter;
}

void ClientRecord::updateFuelSumCounter(){
    fuelSumCounter++;
}

bool ClientRecord::updateAverageFuel(double fuel){
    currentAverageFuel = ( getAverageFuel() + fuel ) / getFuelSumCounter();
    updateFuelSumCounter();
}


