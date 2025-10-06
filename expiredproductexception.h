#ifndef EXPIREDPRODUCTEXCEPTION_H
#define EXPIREDPRODUCTEXCEPTION_H

#include <iostream>
#include <string>
#include <ctime>
#include "medicalproductexception.h"
#include "safedate.h"

class ExpiredProductException : public MedicalProductException
{
private:
    std::string productId;
    SafeDate expirationDate;

public:
    ExpiredProductException(const std::string& productId, const SafeDate& expDate)
        : MedicalProductException("Product " + productId + " is expired"),
        productId(productId), expirationDate(expDate) {}  // Копирование SafeDate

    std::string getProductId() const { return productId; }
    SafeDate getExpirationDate() const { return expirationDate; }
};

#endif // EXPIREDPRODUCTEXCEPTION_H
