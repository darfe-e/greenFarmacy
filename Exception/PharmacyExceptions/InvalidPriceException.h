#ifndef INVALIDPRICEEXCEPTION_H
#define INVALIDPRICEEXCEPTION_H

#include "InvalidProductDataException.h"

class InvalidPriceException : public InvalidProductDataException
{
public:
    InvalidPriceException(double price)
        : InvalidProductDataException("Price", std::to_string(price)) {}

    std::string getErrorCode() const override { return "PRODUCT_PRICE_001"; }
};

#endif // INVALIDPRICEEXCEPTION_H
