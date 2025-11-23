#ifndef SAFEINPUT_H
#define SAFEINPUT_H

#include <iostream>
#include <string>
#include <limits>
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/PharmacyExceptions/InvalidPriceException.h"
#include "Exception/PharmacyExceptions/InvalidProductIdException.h"

class SafeInput {
public:
    static std::string readNonEmptyString(std::istream& is, const std::string& fieldName);

    static double readPositiveDouble(std::istream& is, const std::string& fieldName);

    static int readPositiveInt(std::istream& is, const std::string& fieldName);

    static bool readBoolean(std::istream& is, const std::string& fieldName);

    static std::string readProductId(std::istream& is);

    static void skipLabel(std::istream& is);

    static void clearInputStream(std::istream& is);

private:
    SafeInput() = delete;
};

#endif // SAFEINPUT_H
