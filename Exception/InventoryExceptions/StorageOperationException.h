#ifndef STORAGEOPERATIONEXCEPTION_H
#define STORAGEOPERATIONEXCEPTION_H

#include "InventoryException.h"

class StorageOperationException : public InventoryException
{
private:
    std::string operation;
    std::string location;

public:
    StorageOperationException(const std::string& op, const std::string& loc, const std::string& details)
        : InventoryException("Storage operation '" + op + "' failed at '" + loc + "': " + details),
        operation(op), location(loc) {}

    std::string getErrorCode() const override { return "STORAGE_OPERATION_001"; }
    std::string getOperation() const { return operation; }
    std::string getLocation() const { return location; }
};

#endif // STORAGEOPERATIONEXCEPTION_H
