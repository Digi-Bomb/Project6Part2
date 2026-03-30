#pragma once
#include <iostream>
#include <cstring>

class Packet {
    struct Header {
        bool startFlag;           // True if SOF (Start of Flight)
        bool endFlag;             // True if EOF (End of Flight)
        std::string clientID;        // Unique ID for the flight
        unsigned int packetSize;  // Total size of body
    } Head;
    char* data;         // Telemetry data
    unsigned int crc;   // Cyclic Redundancy Check
    char* txBuffer;

public:
	// Default Constructor - is sizeof(Head) + packetSize + sizeof(crc) correct for total size of serialized packet? i dont think sizeof a class is always accurate
    Packet() : data(nullptr), txBuffer(nullptr), crc(0) { memset(&Head, 0, sizeof(Head)); };

    // Constructor used by Server to DESERIALIZE raw bytes from recvfrom()
    Packet(char* src)
    {
        // memcpy => header
        memcpy(&Head, src, sizeof(Head));

        // memcpy => body
        data = new char[Head.packetSize + 1];
        memcpy(data, src + sizeof(Head), Head.packetSize);
        data[Head.packetSize] = '\0';

        // memcpy => tail/CRC
        memcpy(&crc, src + sizeof(Head) + Head.packetSize, sizeof(unsigned int));

        // set txBuffer pointer to NULL
        txBuffer = nullptr;
    }

    // Destructor to prevent memory leaks
    ~Packet() {
        if (data) delete[] data;
        if (txBuffer) delete[] txBuffer;
    }

    // Getters and Setters
    void setStartFlag(bool val) { Head.startFlag = val; }
    void setEndFlag(bool val) { Head.endFlag = val; }
    void setClientID(const char* id) { Head.clientID; }

    char* getClientID() {
        //return Head.clientID;
    }

    bool getStartFlag() {
        return Head.startFlag;
    }

    bool getEndFlag() {
        return Head.endFlag;
    }

    unsigned int getPacketSize() {
        return Head.packetSize;
    }

    char* getTelemetryData() {
        return this->data;
    }



    void setData(char* srcData, int size) {
      /*  std::cout << "setData called\n";
        std::cout << "size parameter: " << size << std::endl;
        std::cout << "strlen(srcData): " << strlen(srcData) << std::endl;*/
        if (data) delete[] data;
        Head.packetSize = size;
        data = new char[Head.packetSize + 1];
        memcpy(data, srcData, Head.packetSize);
        data[Head.packetSize] = '\0';
    }

    // SERIALIZE: Converts object to byte stream for sendto()
    char* serialize(int& totalSize) {
        //  if (txBuffer) delete[] txBuffer; // Clear previous buffer

        crc = calculateCRC();
        totalSize = sizeof(Head) + Head.packetSize + sizeof(unsigned int);
        txBuffer = new char[totalSize];

        // Copy segments into the flat buffer
        memcpy(txBuffer, &Head, sizeof(Head));
        memcpy(txBuffer + sizeof(Head), data, Head.packetSize);
        memcpy(txBuffer + sizeof(Head) + Head.packetSize, &crc, sizeof(unsigned int));

        return txBuffer;
    }

    unsigned int calculateCRC() {
        return 0xFF00FF00;
    }
};