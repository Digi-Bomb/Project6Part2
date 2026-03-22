#pragma once
#include <string>
#include <fstream>
#include <iostream>

class FileReader {
private:
    char* filePath;
    std::ifstream fileStream;
    int lineNumber;

public:
    FileReader(const char* path);
    ~FileReader();
    bool openFile();
    bool readLine(std::string& lineRead);
    bool isEOF();
    int getLineNumber() const { return lineNumber; }
    const char* getFilePath() const { return filePath; }
};