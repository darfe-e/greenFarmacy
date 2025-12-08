#ifndef MEDICALPRODUCTEXCEPTION_H
#define MEDICALPRODUCTEXCEPTION_H

#include "PharmacyException.h"


class MedicalProductException : public PharmacyException
{
public:
    MedicalProductException(const std::string& message)
        : PharmacyException(message) {}

    std::string getErrorCode() const override { return "PRODUCT_001"; }
};


#endif // MEDICALPRODUCTEXCEPTION_H
