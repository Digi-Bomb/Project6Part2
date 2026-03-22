#include "FileReader.h"

FileReader::FileReader(const char* path)
{
	this->filePath = _strdup(path);
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
	return false;
}

bool FileReader::isEOF()
{
	return false;
}
