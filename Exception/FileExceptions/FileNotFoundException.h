#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H

#include "FileOperationException.h"

class FileNotFoundException : public FileOperationException
{
private:
    std::string filename;

public:
    FileNotFoundException(const std::string& file)
        : FileOperationException("File not found: '" + file + "'"),
        filename(file) {}

    std::string getErrorCode() const override { return "FILE_NOT_FOUND_001"; }
    std::string getFilename() const { return filename; }
};

#endif // FILENOTFOUNDEXCEPTION_H
