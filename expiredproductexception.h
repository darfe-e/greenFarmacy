#ifndef EXPIREDPRODUCTEXCEPTION_H
#define EXPIREDPRODUCTEXCEPTION_H

#include <iostream>

class ExpiredProductException : public MedicalProductException {
private:
    std::string productId;
    std::tm expirationDate;

public:
    ExpiredProductException(const std::string& productId, const std::tm& expDate)
        : MedicalProductException("Product " + productId + " is expired"),
        productId(productId), expirationDate(expDate) {}

    std::string getProductId() const { return productId; }
    std::tm getExpirationDate() const { return expirationDate; }
};

#endif // EXPIREDPRODUCTEXCEPTION_H
