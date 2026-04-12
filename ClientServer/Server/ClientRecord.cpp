#include "ClientRecord.h"

#include <ctime>
#include <iostream>
#include <string>

ClientRecord::ClientRecord(std::string clientID, std::string planeFileName, time_t lastSeen)
    : clientID(clientID),
    planeFileName(planeFileName),
    lastSeen(lastSeen),
    currentAverageFuel(0.0f),
    fuelSumCounter(0),
    currentConsumption(0.0f),
    lastFuel(0.0f),
    hasPrevious(false)
{
}

ClientRecord::~ClientRecord() = default;

float ClientRecord::getAverageFuel() {
    return this->currentAverageFuel;
}

int ClientRecord::getFuelSumCounter() {
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

float ClientRecord::getCurrentConsumption() {
    return this->currentConsumption;
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
    this->fuelSumCounter = 1;
    this->lastFuel = firstPacketFuel;
    this->hasPrevious = true;
}

void ClientRecord::updateFuelConsumption(float fuel) {
    if (hasPrevious) {
        this->currentConsumption = this->lastFuel - fuel;

        // Prevent negative consumption if fuel increases or data is noisy
        if (this->currentConsumption < 0.0f) {
            this->currentConsumption = 0.0f;
        }
    }
    else {
        // First packet has no previous value to compare against
        this->currentConsumption = 0.0f;
        this->hasPrevious = true;
    }

    this->lastFuel = fuel;

    // Optional running average tracking
    this->fuelSumCounter++;
    if (this->fuelSumCounter == 1) {
        this->currentAverageFuel = fuel;
    }
    else {
        this->currentAverageFuel =
            ((this->currentAverageFuel * (this->fuelSumCounter - 1)) + fuel) / this->fuelSumCounter;
    }

    this->lastSeen = std::time(nullptr);

    //char result[26]{};
    //if (ctime_s(result, sizeof(result), &(this->lastSeen)) == 0) {
    //    std::cout << "Current Fuel Consumption: " << this->currentConsumption << std::endl;
    //    std::cout << "Last time: " << result;
    //}
    //else {
    //    std::cout << "Current Fuel Consumption: " << this->currentConsumption << std::endl;
    //    std::cout << "Last time: <unavailable>" << std::endl;
    //}
}