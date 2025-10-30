#ifndef FILEWRITEEXCEPTION_H
#define FILEWRITEEXCEPTION_H

#include "FileWriteException.h"

class FileWriteException : public FileOperationException
{
private:
    std::string filename;

public:
    FileWriteException(const std::string& file, const std::string& details = "")
        : FileOperationException("Write error for file '" + file + "': " + details),
        filename(file) {}

    std::string getErrorCode() const override { return "FILE_WRITE_001"; }
    std::string getFilename() const { return filename; }
};

#endif // FILEWRITEEXCEPTION_H
