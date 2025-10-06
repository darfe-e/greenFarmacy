#ifndef INVALIDPRODUCTDATAEXCEPTION_H
#define INVALIDPRODUCTDATAEXCEPTION_H

#include <iostream>
#include "medicalproductexception.h"
#include <ctime>


class InvalidProductDataException : public MedicalProductException {
public:
    InvalidProductDataException(const std::string& field, const std::string& value)
        : MedicalProductException("Invalid " + field + ": " + value) {}
};
#endif // INVALIDPRODUCTDATAEXCEPTION_H
