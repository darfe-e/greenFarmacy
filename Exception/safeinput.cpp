#include "safeinput.h"
#include <algorithm>
#include <cctype>
#include <locale>

// Для работы с русскими символами
bool isRussianChar(unsigned char c) {
    // Русские буквы в кодировке Windows-1251
    return (c >= 0xC0 && c <= 0xFF) || c == 0xA8 || c == 0xB8;
}

bool isLetter(unsigned char c) {
    return std::isalpha(c) || isRussianChar(c);
}

std::string SafeInput::readNonEmptyString(std::istream& is, const std::string& fieldName)
{
    std::string value;
    std::getline(is, value);

    if (value.empty())
    {
        throw InvalidProductDataException(fieldName, "cannot be empty");
    }

    return value;
}

double SafeInput::readPositiveDouble(std::istream& is, const std::string& fieldName)
{
    double value;
    is >> value;

    if (is.fail())
    {
        clearInputStream(is);
        throw InvalidProductDataException(fieldName, "must be a valid number");
    }

    if (value < 0)
    {
        throw InvalidProductDataException(fieldName, " cannot be negative");
    }

    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

int SafeInput::readPositiveInt(std::istream& is, const std::string& fieldName)
{
    int value;
    is >> value;

    if (is.fail())
    {
        clearInputStream(is);
        throw InvalidProductDataException(fieldName, "must be a valid integer");
    }

    if (value <= 0)
    {
        throw InvalidProductDataException(fieldName, "must be positive");
    }

    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

bool SafeInput::readBoolean(std::istream& is, const std::string& fieldName)
{
    std::string value;
    std::getline(is, value);

    // Приводим к нижнему регистру для сравнения
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);

    if (value == "yes" || value == "true" || value == "1" || value == "да")
    {
        return true;
    }
    else if (value == "no" || value == "false" || value == "0" || value == "нет")
    {
        return false;
    }
    else
    {
        throw InvalidProductDataException(fieldName, "must be 'Yes' or 'No'");
    }
}

std::string SafeInput::readProductId(std::istream& is)
{
    std::string id = readNonEmptyString(is, "Product ID");

    // Дополнительная валидация ID
    if (id.length() < 3)
    {
        throw InvalidProductIdException(id);
    }

    // ПРОВЕРКА НА ТОЛЬКО ЦИФРЫ (может начинаться с нулей)
    for (char c : id)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw InvalidProductIdException(id);
        }
    }

    return id;
}

void SafeInput::skipLabel(std::istream& is)
{
    std::string temp;
    std::getline(is, temp);
}

void SafeInput::clearInputStream(std::istream& is)
{
    is.clear();
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// НОВЫЕ МЕТОДЫ (ИСПРАВЛЕННЫЕ ДЛЯ РУССКИХ СИМВОЛОВ)

bool SafeInput::containsOnlyDigits(const std::string& str) {
    for (unsigned char c : str) {
        if (!std::isdigit(c)) return false;
    }
    return !str.empty();
}

bool SafeInput::containsLetters(const std::string& str) {
    for (unsigned char c : str) {
        if (isLetter(c)) return true;
    }
    return false;
}

bool SafeInput::isValidTextField(const std::string& str) {
    return !str.empty() && containsLetters(str) && !containsOnlyDigits(str);
}

void SafeInput::validateTextField(const std::string& str, const std::string& fieldName) {
    if (str.empty()) {
        throw InvalidProductDataException(fieldName, "cannot be empty");
    }
    if (!containsLetters(str)) {
        throw InvalidProductDataException(fieldName, "must contain letters");
    }
    if (containsOnlyDigits(str)) {
        throw InvalidProductDataException(fieldName, "cannot contain only digits");
    }
}

void SafeInput::validateProductId(const std::string& id) {
    if (id.empty()) {
        throw InvalidProductDataException("ID", "cannot be empty");
    }
    if (id.length() < 3) {
        throw InvalidProductDataException("ID", "must be at least 3 digits long");
    }
    if (!containsOnlyDigits(id)) {
        throw InvalidProductDataException("ID", "must contain only digits");
    }
}
