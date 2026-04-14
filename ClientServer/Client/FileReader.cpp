#include "FileReader.h"
/**
*@file FileReader.cpp
*@brief The FileReader c++ logic. Helps each client read a telemetry file, parsing data line by line.
*/

/**
* @brief FileReader constructor, stores the filepath to read/parse from.
* @param [const char*] path, the filepath to open.
*/
FileReader::FileReader(const char* path)
{
	this->filePath = _strdup(path);
	this->lineNumber = 1;
}

/**
* @brief FileReader destructor, closes open files, empties local filepath data.
*/
FileReader::~FileReader()
{
    if (this->fileStream.is_open()) {
        this->fileStream.close();
    }

    if (this->filePath != nullptr) {
        free(this->filePath);
        this->filePath = nullptr;
    }
}

/**
* @brief The openFile function, aptly named, opens a file using the stored/initialized filepath
*/
bool FileReader::openFile()
{

    if (this->filePath == nullptr) {
        return false;
    }

    this->fileStream.open(this->filePath);

    return this->fileStream.is_open();
}

/**
* @brief The readLine function, aptly named, reads a line from the previously opened file, storing that line inside of the passed string
* @param [string&] lineRead, the string to store the current line's data
* @return [bool] Returns a boolean of whether or not there is a line to read
*/
bool FileReader::readLine(std::string& lineRead)
{
    if (this->fileStream.is_open() && !this->isEOF()) {
        if (std::getline(this->fileStream, lineRead)) {
            this->lineNumber++;
            return true;
        }
    }
    return false;
}

/**
* @brief The isEOF function, aptly named, returns a boolean value of whether or not the end of the file has been reached
* @return [boo] Returns a boolean of whether or not the end of the file has been reached, or is about to be reached.
*/
bool FileReader::isEOF()
{

    // clear failbit (might not be an error when opening file through cmd, but these lines enable manual testing)
    this->fileStream.clear();         

    if (!this->fileStream.is_open()) {
        
        return true;
    }

    if (this->fileStream.eof()) {
      
        return true;
    }

    while (isspace(this->fileStream.peek())) {
        this->fileStream.get();
    }

    int nextChar = this->fileStream.peek();
    if (nextChar == EOF) {
       
        return true;
    }

    return false;
}
