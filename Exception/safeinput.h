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
    // Безопасный ввод строки (не пустая)
    static std::string readNonEmptyString(std::istream& is, const std::string& fieldName);

    // Безопасный ввод числа с плавающей точкой
    static double readPositiveDouble(std::istream& is, const std::string& fieldName);

    // Безопасный ввод целого числа
    static int readPositiveInt(std::istream& is, const std::string& fieldName);

    // Безопасный ввод булевого значения
    static bool readBoolean(std::istream& is, const std::string& fieldName);

    // Безопасный ввод ID продукта
    static std::string readProductId(std::istream& is);

    // Пропуск метки
    static void skipLabel(std::istream& is);

    // Очистка потока при ошибке
    static void clearInputStream(std::istream& is);

private:
    SafeInput() = delete;
};

#endif // SAFEINPUT_H
