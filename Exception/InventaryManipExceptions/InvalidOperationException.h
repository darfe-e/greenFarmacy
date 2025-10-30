#ifndef INVALIDOPERATIONEXCEPTION_H
#define INVALIDOPERATIONEXCEPTION_H

#include "Exception/PharmacyExceptions/PharmacyException.h"

class InvalidOperationException : public PharmacyException
{
private:
    std::string operation;

public:
    InvalidOperationException(const std::string& op, const std::string& details)
        : PharmacyException("Invalid operation '" + op + "': " + details),
        operation(op) {}

    std::string getErrorCode() const override { return "OPERATION_INVALID_001"; }
    std::string getOperation() const { return operation; }
};

#endif // INVALIDOPERATIONEXCEPTION_H
