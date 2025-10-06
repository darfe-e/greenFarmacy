#ifndef MEDICALPRODUCTEXCEPTION_H
#define MEDICALPRODUCTEXCEPTION_H

#include <iostream>

class MedicalProductException : public std::runtime_error {
public:
    MedicalProductException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif // MEDICALPRODUCTEXCEPTION_H
