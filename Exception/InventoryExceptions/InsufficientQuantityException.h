#ifndef INSUFFICIENTQUANTITYEXCEPTION_H
#define INSUFFICIENTQUANTITYEXCEPTION_H

#include "InventoryException.h"

class InsufficientQuantityException : public InventoryException
{
private:
    std::string productId;
    int requested;
    int available;

public:
    InsufficientQuantityException(const std::string& id, int req, int avail)
        : InventoryException("Insufficient quantity for product '" + id + "'. Requested: " +
                             std::to_string(req) + ", Available: " + std::to_string(avail)),
        productId(id), requested(req), available(avail) {}

    std::string getErrorCode() const override { return "INVENTORY_QUANTITY_001"; }
    std::string getProductId() const { return productId; }
    int getRequestedQuantity() const { return requested; }
    int getAvailableQuantity() const { return available; }
};

#endif // INSUFFICIENTQUANTITYEXCEPTION_H
