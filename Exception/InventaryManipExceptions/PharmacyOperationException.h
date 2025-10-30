#ifndef PHARMACYOPERATIONEXCEPTION_H
#define PHARMACYOPERATIONEXCEPTION_H

#include "Exception/PharmacyExceptions/PharmacyException.h"

class PharmacyOperationException : public PharmacyException
{
private:
    std::string pharmacyId;

public:
    PharmacyOperationException(const std::string& id, const std::string& message)
        : PharmacyException("Pharmacy '" + id + "': " + message),
        pharmacyId(id) {}

    std::string getErrorCode() const override { return "PHARMACY_OPERATION_001"; }
    std::string getPharmacyId() const { return pharmacyId; }
};

#endif // PHARMACYOPERATIONEXCEPTION_H
