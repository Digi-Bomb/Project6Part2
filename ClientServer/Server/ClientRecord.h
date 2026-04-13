#pragma once
#include <string>
#include <iostream>

/**
*@file ClientRecord.h
* @brief The ClientRecord class' header. Accepts parsed packet data and enacts logic on it, generating an ongoing fuel consumption average for each client
*/

/**
* @brief ClientRecord class. Private variables listed here:
* @param [string] clientID represents the each client's unique ID
* @param [string] planeFileName represents the flight file whose data is being transmitted
* @param [time_t] lastSeen represents the most recent datetime in which the server has received a message from each client
* @param [float] currentAverageFuel maintains the average fuel value of each client transmission
* @param [float] currentConsumption maintains the average fuel consumption of each client
* @param [bool] hasPrevious assures whether or not a client has a previous entry
* @param [int] fuelSumCounter maintains the number of transmissions received from the client
* @param [float] lastFuel maintains the previous fuel consumption average (if it exists)
*/
class ClientRecord
{
private:
    std::string clientID;
    std::string planeFileName;
    time_t lastSeen;
    float currentAverageFuel;
    float currentConsumption;
    bool hasPrevious;
    int fuelSumCounter;
    float lastFuel;

public:
  
    ~ClientRecord();
    //ClientRecord();
    ClientRecord(std::string clientID, std::string planeFileName, time_t lastSeen);

    void setClientID(std::string clientID);
    void setPlaneFlightName(std::string planeFileName);
    void setTimeLastSeen(time_t lastSeen);
    void setFuel(float firstPacketFuel); //Truly only needed for initial fuel setting
    
    std::string getClientID();
    std::string getFlightName();
    time_t getLastSeen();
    float getCurAvgFuel();
    int getFuelSum();
    float getCurrentConsumption();


    void updateFuelConsumption(float fuel);
    float getAverageFuel();
    int getFuelSumCounter();
};
