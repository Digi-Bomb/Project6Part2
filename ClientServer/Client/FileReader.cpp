#include "FileReader.h"
#include <cstring>

FileReader::FileReader(const char* path)
{
	this->filePath = strdup(path);
	this->lineNumber = 1;
}

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

bool FileReader::openFile()
{

    if (this->filePath == nullptr) {
        return false;
    }

    this->fileStream.open(this->filePath);

    return this->fileStream.is_open();
}

bool FileReader::readLine(std::string& lineRead)
{
    if (this->fileStream.is_open() && std::getline(this->fileStream, lineRead)) {
        this->lineNumber++;
        return true;
    }
    return false;
}

bool FileReader::isEOF()
{
    if (!this->fileStream.is_open()) {
        return true;
    }

    return this->fileStream.eof() || this->fileStream.peek() == EOF;
}
