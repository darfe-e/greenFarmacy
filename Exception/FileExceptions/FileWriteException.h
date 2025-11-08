#ifndef FILEWRITEEXCEPTION_H
#define FILEWRITEEXCEPTION_H

#include "FileOperationException.h"
#include <string>

class FileWriteException : public FileOperationException
{
private:
    std::string filename;

public:
    FileWriteException(const std::string& file, const std::string& details = "")
        : FileOperationException("Write error for file '" + file + "': " + details),
        filename(file) {}

    std::string getFilename() const { return filename; }
};

#endif // FILEWRITEEXCEPTION_H
