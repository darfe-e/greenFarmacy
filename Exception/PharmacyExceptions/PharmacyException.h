#ifndef PHARMACYEXCEPTION_H
#define PHARMACYEXCEPTION_H

#include <iostream>


class PharmacyException : public std::runtime_error
{
public:
    PharmacyException(const std::string& message)
        : std::runtime_error(message) {}

    virtual std::string getErrorCode() const { return "PHARMACY_001"; }
    virtual ~PharmacyException() = default;
};



#endif // PHARMACYEXCEPTION_H
