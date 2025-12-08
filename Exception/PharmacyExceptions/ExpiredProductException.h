#ifndef EXPIREDPRODUCTEXCEPTION_H
#define EXPIREDPRODUCTEXCEPTION_H

#include "MedicalProductException.h"
#include "my_inheritence/safedate.h"

class ExpiredProductException : public MedicalProductException
{
private:
    std::string productId;
    SafeDate expirationDate;

public:
    ExpiredProductException(const std::string& id, const SafeDate& expDate)
        : MedicalProductException("Product '" + id + "' is expired"),
        productId(id), expirationDate(expDate) {}

    std::string getErrorCode() const override { return "PRODUCT_EXPIRED_001"; }
    std::string getProductId() const { return productId; }
    SafeDate getExpirationDate() const { return expirationDate; }
};

#endif // EXPIREDPRODUCTEXCEPTION_H
