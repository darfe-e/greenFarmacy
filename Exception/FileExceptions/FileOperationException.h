#ifndef FILEOPERATIONEXCEPTION_H
#define FILEOPERATIONEXCEPTION_H

#include "Exception/PharmacyExceptions/PharmacyException.h"

class FileOperationException : public PharmacyException
{
public:
    FileOperationException(const std::string& message)
        : PharmacyException(message) {}

    std::string getErrorCode() const override { return "FILE_001"; }
};


#endif // FILEOPERATIONEXCEPTION_H
