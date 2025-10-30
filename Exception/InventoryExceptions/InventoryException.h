#ifndef INVENTORYEXCEPTION_H
#define INVENTORYEXCEPTION_H

#include "Exception/PharmacyExceptions/PharmacyException.h"

class InventoryException : public PharmacyException
{
public:
    InventoryException(const std::string& message)
        : PharmacyException(message) {}

    std::string getErrorCode() const override { return "INVENTORY_001"; }
};


#endif // INVENTORYEXCEPTION_H
