#include <string>
#include <iostream>
#include "ClientRecord.h"

ClientRecord::ClientRecord(std::string clientID, std::string planeFileName, time_t lastSeen){

    this->clientID = clientID;

    this->planeFileName = planeFileName;

    this->lastSeen = lastSeen;
    
    this->currentAverageFuel = 0;

    this->fuelSumCounter = 0; 
}

//TODO: Need to build the desctructor (if we find memory issues)
ClientRecord::~ClientRecord() {

}
float ClientRecord::getAverageFuel(){
    return this->currentAverageFuel;
}

int ClientRecord::getFuelSumCounter(){
    return this->fuelSumCounter;
}


std::string ClientRecord::getClientID() {
    return this->clientID;
}

time_t ClientRecord::getLastSeen() {
    return this->lastSeen;
}

float ClientRecord::getCurAvgFuel() {
    return this->currentAverageFuel;
}

int ClientRecord::getFuelSum() {
    return this->fuelSumCounter;
}

void ClientRecord::setClientID(std::string clientID) {
    this->clientID = clientID;

}

void ClientRecord::setPlaneFlightName(std::string planeFileName) {
    this->planeFileName = planeFileName;
}

void ClientRecord::setTimeLastSeen(time_t lastSeen) {
    this->lastSeen = lastSeen;
}

void ClientRecord::setFuel(float firstPacketFuel) {
    this->currentAverageFuel = firstPacketFuel;
}

void ClientRecord::updateFuelSumCounter(){
    this->fuelSumCounter++;
}

void ClientRecord::updateAverageFuel(float fuel){

    if (this->fuelSumCounter == 0) 
        this->currentAverageFuel = fuel;
    
    else 
        this->currentAverageFuel = (getAverageFuel() + fuel) / getFuelSumCounter();
    
    std::cout << "Current Average Fuel Level: " << this->currentAverageFuel << std::endl;

    char result[26];
    ctime_s(result, sizeof(result), &(this->lastSeen));
    std::cout << "Last time: " << result << std::endl;
    updateFuelSumCounter();
    std::cout << "After Fuel Sum: " << this->fuelSumCounter << std::endl;
}


