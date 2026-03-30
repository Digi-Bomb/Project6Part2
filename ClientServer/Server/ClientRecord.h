#pragma once
#include <string>
#include <iostream>

class ClientRecord
{
private:
    std::string clientID;
    std::string planeFileName;
    time_t lastSeen;
    float currentAverageFuel;
    int fuelSumCounter;

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


    void updateAverageFuel(float fuel);
    void updateFuelSumCounter();
    float getAverageFuel();
    int getFuelSumCounter();
};
