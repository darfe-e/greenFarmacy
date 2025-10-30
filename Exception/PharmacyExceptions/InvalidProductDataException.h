#ifndef INVALIDPRODUCTDATAEXCEPTION_H
#define INVALIDPRODUCTDATAEXCEPTION_H

#include "MedicalProductException.h"

class InvalidProductDataException : public MedicalProductException
{
private:
    std::string fieldName;
    std::string invalidValue;

public:
    InvalidProductDataException(const std::string& field, const std::string& value)
        : MedicalProductException("Invalid " + field + ": '" + value + "'"),
        fieldName(field), invalidValue(value) {}

    std::string getErrorCode() const override { return "PRODUCT_VALIDATION_001"; }
    std::string getFieldName() const { return fieldName; }
    std::string getInvalidValue() const { return invalidValue; }
};

#endif // INVALIDPRODUCTDATAEXCEPTION_H
