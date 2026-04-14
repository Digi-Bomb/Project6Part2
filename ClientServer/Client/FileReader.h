#pragma once
#include <string>
#include <fstream>
#include <iostream>

/**
*@file FileReader.h
*@brief The FileReader's header file. Defines functions and variables for that each client will use when parsing file data
*/

/**
* @brief The FileReader Class. Private variables listed here:
* 

____________________________________________________________________
*@param [char*] filePath, the filepath where the file to be read is stored
*@param [ifstream] fileStream, a filestream of the opened filepath
*@param [int] lineNumber, the current line number being read from/within the file.
*/
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