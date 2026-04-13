#include "ClientRecord.h"

#include <ctime>
#include <iostream>
#include <string>

/**
*@file ClientRecord.cpp
* @brief The ClientRecord c++ logic. Executes logical executions on various types of received packet data
*/

/**
    * @brief The ClientRecord Constructor intializes a ClientRecord class for each client who connects to the server
    * @param [string] clientID, as it sounds, holds the ID of the client who's clientRecord is being requested
    * @param [string] planeFileName, as it sounds, holds the name of the flight who's data is being transmitted
    * @param [time_t] lastSeen, as it sounds, holds the time and date of when the most recent packet was received by the server for this particular client.
*/
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

/**
    * @brief getAverageFuel returns this particular client's average fuel consumption
    * @return returns this particular client's average fuel consumption
*/
float ClientRecord::getAverageFuel() {
    return this->currentAverageFuel;
}

/**
    * @brief getFuelSumCounter returns this particular client's current fuel counter
    * @return returns this particular client's current fuel counter
*/
int ClientRecord::getFuelSumCounter() {
    return this->fuelSumCounter;
}

/**
    * @brief getFlightName returns this particular client's flight name (the file whose data is being transmitted)
    * @return returns this particular client's flight name (the file whose data is being transmitted)
*/
std::string ClientRecord::getFlightName() {
    return this->planeFileName;
}

/**
    * @brief getClientID returns this particular client's ID
    * @return returns this particular client's ID
*/
std::string ClientRecord::getClientID() {
    return this->clientID;
}

/**
    * @brief getLastSeen returns the date and time of the last received packet for this particular client
    * @return returns the date and time of the last received packet for this particular client
*/
time_t ClientRecord::getLastSeen() {
    return this->lastSeen;
}

/**
    * @brief getCurAvgFuel returns this particular client's average fuel consumption
    * @return returns this particular client's average fuel consumption
    DUPLICATE FUNCTION
*/
float ClientRecord::getCurAvgFuel() {
    return this->currentAverageFuel;
}

/**
    * @brief getFuelSumCounter returns this particular client's current fuel counter
    * @return returns this particular client's current fuel counter
    DUPLICATE FUNCTION
*/
int ClientRecord::getFuelSum() {
    return this->fuelSumCounter;
}

/**
    * @brief getAverageFuel returns this particular client's average fuel consumption
    * @return returns this particular client's average fuel consumption
*/
float ClientRecord::getCurrentConsumption() {
    return this->currentConsumption;
}

/**
    * @brief setClientID is a setter function that sets the respective client's ID to the data receieved
    * @param [string] clientID, as it sounds, holds the ID of the client who's clientRecord is being requested
*/
void ClientRecord::setClientID(std::string clientID) {
    this->clientID = clientID;
}

/**
    * @brief setPlaneFlightName is a setter function that sets the respective client's flight name to the data receieved
    * @param [string] planeFileName, as it sounds, holds the name of the flight who's data is being transmitted
*/
void ClientRecord::setPlaneFlightName(std::string planeFileName) {
    this->planeFileName = planeFileName;
}

/**
    * @brief setTimeLastSeen is a setter function that sets the respective client's time of last seen to the data receieved
    * @param [time_t] lastSeen, as it sounds, holds the time and date of when the most recent packet was received by the server for this particular client.
*/
void ClientRecord::setTimeLastSeen(time_t lastSeen) {
    this->lastSeen = lastSeen;
}

/**
    * @brief setFuel is a setter function that initializes a client's fuel to 0 when receiving an SOF packet
    * @param [float] firstPacketFuel representes the fuel level that the flight is starting with
    SETS THE FIRST AVERAGE CONSUMPTION
    SETS THE FUEL SUM COUNTER TO 1 (received a single transmission)
    ITIALIZES THE PREVIOUS FUEL VALUE
    ENSURES SERVER KNOWS THAT THIS CLIENT HAS SENT AT LEAST 1 PACKET THUS FAR
*/
void ClientRecord::setFuel(float firstPacketFuel) {
    this->currentAverageFuel = firstPacketFuel;
    this->fuelSumCounter = 1;
    this->lastFuel = firstPacketFuel;
    this->hasPrevious = true;
}

/**
    * @brief updateFuelConsumption is a setter/updater function that executes simple logic to determine the average fuel consumption between packets. Stores an ongoing fuel consumption average for each client.
    * @param [float] fuel holds the parsed fuel value from the respective client's received packet
*/
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

}