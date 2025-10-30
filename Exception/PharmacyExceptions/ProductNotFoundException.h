#ifndef PRODUCTNOTFOUNDEXCEPTION_H
#define PRODUCTNOTFOUNDEXCEPTION_H

#include "MedicalProductException.h"

class ProductNotFoundException : public MedicalProductException
{
private:
    std::string productId;

public:
    ProductNotFoundException(const std::string& id)
        : MedicalProductException("Product not found: '" + id + "'"),
        productId(id) {}

    std::string getErrorCode() const override { return "PRODUCT_NOT_FOUND_001"; }
    std::string getProductId() const { return productId; }
};

#endif // PRODUCTNOTFOUNDEXCEPTION_H
