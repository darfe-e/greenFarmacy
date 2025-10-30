#ifndef INVALIDPRODUCTIDEXCEPTION_H
#define INVALIDPRODUCTIDEXCEPTION_H

#include "InvalidProductDataException.h"

class InvalidProductIdException : public InvalidProductDataException
{
public:
    InvalidProductIdException(const std::string& id)
        : InvalidProductDataException("Product ID", id) {}

    std::string getErrorCode() const override { return "PRODUCT_ID_001"; }
};

#endif // INVALIDPRODUCTIDEXCEPTION_H
