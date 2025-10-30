#ifndef NEGATIVEQUANTITYEXCEPTION_H
#define NEGATIVEQUANTITYEXCEPTION_H

#include "InventoryException.h"

class NegativeQuantityException : public InventoryException
{
public:
    NegativeQuantityException(int quantity)
        : InventoryException("Quantity cannot be negative: " + std::to_string(quantity)) {}

    std::string getErrorCode() const override { return "INVENTORY_QUANTITY_002"; }
};

#endif // NEGATIVEQUANTITYEXCEPTION_H
