#pragma once
#include <iostream>
#include <cstring>
#include <stdexcept>

class Packet
{
public:
#pragma pack(push, 1)
    struct Header
    {
        bool startFlag;
        bool endFlag;
        char clientID[37];
        unsigned int packetSize;
    };
#pragma pack(pop)

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

        // Validate CRC
        unsigned int expectedCRC = calculateCRC();
        if (crc != expectedCRC)
        {
            delete[] data;
            data = nullptr;
            throw std::runtime_error("CRC mismatch - corrupted packet");
        }
    }

    ~Packet()
    {
        delete[] data;
        delete[] txBuffer;
    }

    // Delete copy operations to prevent double-free from raw pointers
    Packet(const Packet &) = delete;
    Packet &operator=(const Packet &) = delete;

    // Allow move operations
    Packet(Packet &&other) noexcept
        : Head(other.Head), data(other.data), crc(other.crc), txBuffer(other.txBuffer)
    {
        other.data = nullptr;
        other.txBuffer = nullptr;
    }

    Packet &operator=(Packet &&other) noexcept
    {
        if (this != &other)
        {
            delete[] data;
            delete[] txBuffer;
            Head = other.Head;
            data = other.data;
            crc = other.crc;
            txBuffer = other.txBuffer;
            other.data = nullptr;
            other.txBuffer = nullptr;
        }
        return *this;
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
        unsigned int result = 0xFFFFFFFF;
        // CRC over header
        const unsigned char *headerBytes = reinterpret_cast<const unsigned char *>(&Head);
        for (unsigned int i = 0; i < sizeof(Head); i++)
        {
            result ^= headerBytes[i];
            for (int bit = 0; bit < 8; bit++)
            {
                if (result & 1)
                    result = (result >> 1) ^ 0xEDB88320;
                else
                    result >>= 1;
            }
        }
        // CRC over data
        if (data != nullptr)
        {
            for (unsigned int i = 0; i < Head.packetSize; i++)
            {
                result ^= static_cast<unsigned char>(data[i]);
                for (int bit = 0; bit < 8; bit++)
                {
                    if (result & 1)
                        result = (result >> 1) ^ 0xEDB88320;
                    else
                        result >>= 1;
                }
            }
        }
        return result ^ 0xFFFFFFFF;
    }
};