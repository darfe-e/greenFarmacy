#ifndef DUPLICATEPRODUCTEXCEPTION_H
#define DUPLICATEPRODUCTEXCEPTION_H

#include "MedicalProductException.h"

class DuplicateProductException : public MedicalProductException
{
private:
    std::string productId;

public:
    DuplicateProductException(const std::string& id)
        : MedicalProductException("Product with ID '" + id + "' already exists"),
        productId(id) {}

    std::string getErrorCode() const override { return "PRODUCT_DUPLICATE_001"; }
    std::string getProductId() const { return productId; }
};

#endif // DUPLICATEPRODUCTEXCEPTION_H
