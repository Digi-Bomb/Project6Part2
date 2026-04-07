#pragma once
#include <iostream>
#include <cstring>
#include <stdexcept>

class Packet
{
public:
    struct Header
    {
        bool startFlag;
        bool endFlag;
        char clientID[37];
        unsigned int packetSize;
    };

private:
    Header Head;
    char *data;
    unsigned int crc;
    char *txBuffer;

public:
    Packet() : data(nullptr), txBuffer(nullptr), crc(0)
    {
        memset(&Head, 0, sizeof(Head));
    }

    Packet(const char *src, int bytesReceived) : data(nullptr), txBuffer(nullptr), crc(0)
    {
        if (bytesReceived < static_cast<int>(sizeof(Head) + sizeof(unsigned int)))
        {
            throw std::runtime_error("Packet too small");
        }

        memcpy(&Head, src, sizeof(Head));

        int expectedSize = static_cast<int>(sizeof(Head) + Head.packetSize + sizeof(unsigned int));
        if (Head.packetSize > 100000 || bytesReceived < expectedSize)
        {
            throw std::runtime_error("Invalid packet size");
        }

        data = new char[Head.packetSize + 1];
        memcpy(data, src + sizeof(Head), Head.packetSize);
        data[Head.packetSize] = '\0';

        memcpy(&crc, src + sizeof(Head) + Head.packetSize, sizeof(unsigned int));
    }

    ~Packet()
    {
        delete[] data;
        delete[] txBuffer;
    }

    static constexpr unsigned int getHeaderSize()
    {
        return sizeof(Header);
    }

    static constexpr unsigned int getTailSize()
    {
        return sizeof(unsigned int);
    }

    void setStartFlag(bool val) { Head.startFlag = val; }
    void setEndFlag(bool val) { Head.endFlag = val; }
    void setClientID(const char *id) { strncpy_s(Head.clientID, sizeof(Head.clientID), id, 36); }

    char *getClientID() { return Head.clientID; }
    bool getStartFlag() { return Head.startFlag; }
    bool getEndFlag() { return Head.endFlag; }
    unsigned int getPacketSize() { return Head.packetSize; }
    char *getTelemetryData() { return data; }

    void setData(char *srcData, int size)
    {
        delete[] data;
        Head.packetSize = size;
        data = new char[Head.packetSize + 1];
        memcpy(data, srcData, Head.packetSize);
        data[Head.packetSize] = '\0';
    }

    char *serialize(int &totalSize)
    {
        delete[] txBuffer;

        crc = calculateCRC();
        totalSize = sizeof(Head) + Head.packetSize + sizeof(unsigned int);
        txBuffer = new char[totalSize];

        memcpy(txBuffer, &Head, sizeof(Head));
        memcpy(txBuffer + sizeof(Head), data, Head.packetSize);
        memcpy(txBuffer + sizeof(Head) + Head.packetSize, &crc, sizeof(unsigned int));

        return txBuffer;
    }

    unsigned int calculateCRC()
    {
        return 0xFF00FF00;
    }
};