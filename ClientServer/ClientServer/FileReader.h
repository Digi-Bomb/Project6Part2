#pragma once
#include <string>
#include <fstream>
#include <iostream>

class FileReader {
private:
    std::string filePath;
    std::ifstream fileStream;
    int lineNumber;

public:
    FileReader(const std::string& path);
    ~FileReader();
    bool openFile();
    bool readLine(std::string& lineRead);
    bool isEOF();
    int getLineNumber() const { return lineNumber; }
    std::string getFilePath() const { return filePath; }
};