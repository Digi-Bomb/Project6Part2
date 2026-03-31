#include <string>
#include <iostream>
#include "ClientRecord.h"

ClientRecord::ClientRecord(std::string clientID, std::string planeFileName, time_t lastSeen){

    this->clientID = clientID;

    this->planeFileName = planeFileName;

    this->lastSeen = lastSeen;
    
    this->currentAverageFuel = 0;

    this->fuelSumCounter = 0; 

    this->currentConsumption = 0.0f;

    this->hasPrevious = false;
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

std::string ClientRecord::getFlightName() {
    return this->planeFileName;
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

float ClientRecord::getCurrentConsumption() {
    return this->currentConsumption;
}


void ClientRecord::updateFuelConsumption(float fuel){
    if (hasPrevious) {
        this->currentConsumption = this->lastFuel - fuel;

        // prevent negative consumption
        if (this->currentConsumption < 0) {
            this->currentConsumption = 0;
        }
    }
    else {
        // first packet
        this->currentConsumption = 0;
        hasPrevious = true;
    }

    this->lastFuel = fuel;

    // update timestamp
    this->lastSeen = time(nullptr);

    char result[26];
    ctime_s(result, sizeof(result), &(this->lastSeen));

    std::cout << "Current Fuel Consumption: "
        << this->currentConsumption << std::endl;

    std::cout << "Last time: " << result << std::endl;
}


