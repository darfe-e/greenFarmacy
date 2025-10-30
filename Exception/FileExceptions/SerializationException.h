#ifndef SERIALIZATIONEXCEPTION_H
#define SERIALIZATIONEXCEPTION_H

#include "FileOperationException.h"

class SerializationException : public FileOperationException
{
private:
    std::string objectType;

public:
    SerializationException(const std::string& type, const std::string& details)
        : FileOperationException("Serialization failed for " + type + ": " + details),
        objectType(type) {}

    std::string getErrorCode() const override { return "SERIALIZATION_001"; }
    std::string getObjectType() const { return objectType; }
};

#endif // SERIALIZATIONEXCEPTION_H
