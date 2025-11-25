#ifndef FILEPARSEEXCEPTION_H
#define FILEPARSEEXCEPTION_H

#include "FileOperationException.h"

class FileParseException : public FileOperationException
{
private:
    std::string filename;

public:
    FileParseException(const std::string& file)
        : FileOperationException("Parse error in file '" + file ),
        filename(file){}

    std::string getErrorCode() const override { return "FILE_PARSE_001"; }
    std::string getFilename() const { return filename; }

};

#endif // FILEPARSEEXCEPTION_H
