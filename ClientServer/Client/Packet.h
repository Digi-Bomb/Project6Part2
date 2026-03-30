#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <cstdint>

class Packet {
private:
    struct Header {
        uint8_t startFlag;
        uint8_t endFlag;
        uint32_t clientIDSize;
        uint32_t packetSize;
    } Head;

    std::string clientID;
    char* data;
    uint32_t crc;
    char* txBuffer;

public:
    Packet() : clientID(""), data(nullptr), crc(0), txBuffer(nullptr) {
        Head.startFlag = 0;
        Head.endFlag = 0;
        Head.clientIDSize = 0;
        Head.packetSize = 0;
    }

    Packet(const char* src) : clientID(""), data(nullptr), crc(0), txBuffer(nullptr) {
        int offset = 0;

        std::memcpy(&Head.startFlag, src + offset, sizeof(Head.startFlag));
        offset += sizeof(Head.startFlag);

        std::memcpy(&Head.endFlag, src + offset, sizeof(Head.endFlag));
        offset += sizeof(Head.endFlag);

        std::memcpy(&Head.clientIDSize, src + offset, sizeof(Head.clientIDSize));
        offset += sizeof(Head.clientIDSize);

        std::memcpy(&Head.packetSize, src + offset, sizeof(Head.packetSize));
        offset += sizeof(Head.packetSize);

        clientID.assign(src + offset, Head.clientIDSize);
        offset += Head.clientIDSize;

        data = new char[Head.packetSize + 1];
        std::memcpy(data, src + offset, Head.packetSize);
        data[Head.packetSize] = '\0';
        offset += Head.packetSize;

        std::memcpy(&crc, src + offset, sizeof(crc));
    }

    ~Packet() {
        if (data) delete[] data;
        if (txBuffer) delete[] txBuffer;
    }

    void setStartFlag(bool val) { Head.startFlag = val ? 1 : 0; }
    void setEndFlag(bool val) { Head.endFlag = val ? 1 : 0; }

    void setClientID(const std::string& id) {
        clientID = id;
        Head.clientIDSize = static_cast<uint32_t>(clientID.size());
    }

    const std::string& getClientID() const {
        return clientID;
    }

    bool getStartFlag() const { return Head.startFlag != 0; }
    bool getEndFlag() const { return Head.endFlag != 0; }
    uint32_t getPacketSize() const { return Head.packetSize; }
    char* getTelemetryData() const { return data; }

    void setData(const char* srcData, int size) {
        if (data) delete[] data;
        Head.packetSize = static_cast<uint32_t>(size);
        data = new char[Head.packetSize + 1];
        std::memcpy(data, srcData, Head.packetSize);
        data[Head.packetSize] = '\0';
    }

    char* serialize(int& totalSize) {
        if (txBuffer) {
            delete[] txBuffer;
            txBuffer = nullptr;
        }

        Head.clientIDSize = static_cast<uint32_t>(clientID.size());
        crc = calculateCRC();

        totalSize =
            sizeof(Head.startFlag) +
            sizeof(Head.endFlag) +
            sizeof(Head.clientIDSize) +
            sizeof(Head.packetSize) +
            static_cast<int>(Head.clientIDSize) +
            static_cast<int>(Head.packetSize) +
            sizeof(crc);

        txBuffer = new char[totalSize];
        int offset = 0;

        std::memcpy(txBuffer + offset, &Head.startFlag, sizeof(Head.startFlag));
        offset += sizeof(Head.startFlag);

        std::memcpy(txBuffer + offset, &Head.endFlag, sizeof(Head.endFlag));
        offset += sizeof(Head.endFlag);

        std::memcpy(txBuffer + offset, &Head.clientIDSize, sizeof(Head.clientIDSize));
        offset += sizeof(Head.clientIDSize);

        std::memcpy(txBuffer + offset, &Head.packetSize, sizeof(Head.packetSize));
        offset += sizeof(Head.packetSize);

        if (Head.clientIDSize > 0) {
            std::memcpy(txBuffer + offset, clientID.data(), Head.clientIDSize);
            offset += Head.clientIDSize;
        }

        if (Head.packetSize > 0) {
            std::memcpy(txBuffer + offset, data, Head.packetSize);
            offset += Head.packetSize;
        }

        std::memcpy(txBuffer + offset, &crc, sizeof(crc));

        return txBuffer;
    }

    uint32_t calculateCRC() {
        return 0xFF00FF00;
    }
};