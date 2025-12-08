#include "safeinput.h"                      // Включение заголовочных файлов
#include <algorithm>
#include <cctype>
#include <locale>

bool isRussianChar(unsigned char c)        // Проверка на русский символ (Windows-1251)
{
    return (c >= 0xC0 && c <= 0xFF) || c == 0xA8 || c == 0xB8;
}

bool isLetter(unsigned char c)             // Проверка на букву (латинскую или русскую)
{
    return std::isalpha(c) || isRussianChar(c);
}

std::string SafeInput::readNonEmptyString(std::istream& is, const std::string& fieldName)
{
    std::string value;
    std::getline(is, value);

    if (value.empty())
        throw InvalidProductDataException(fieldName, "cannot be empty");

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
        throw InvalidProductDataException(fieldName, " cannot be negative");

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
        throw InvalidProductDataException(fieldName, "must be positive");

    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

bool SafeInput::readBoolean(std::istream& is, const std::string& fieldName)
{
    std::string value;
    std::getline(is, value);

    std::transform(value.begin(), value.end(), value.begin(), ::tolower); // Приведение к нижнему регистру

    if (value == "yes" || value == "true" || value == "1" || value == "да")
        return true;
    else if (value == "no" || value == "false" || value == "0" || value == "нет")
        return false;
    else
        throw InvalidProductDataException(fieldName, "must be 'Yes' or 'No'");
}

std::string SafeInput::readProductId(std::istream& is)
{
    std::string id = readNonEmptyString(is, "Product ID");

    if (id.length() < 3)
        throw InvalidProductDataException(id, "Неверное id!");

    for (char c : id)                      // Проверка, что ID состоит только из цифр
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
            throw InvalidProductDataException(id, "Неверное id!");
    }

    return id;
}

void SafeInput::skipLabel(std::istream& is) // Пропуск метки (строки) во входном потоке
{
    std::string temp;
    std::getline(is, temp);
}

void SafeInput::clearInputStream(std::istream& is) // Очистка состояния потока ввода
{
    is.clear();
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool SafeInput::containsOnlyDigits(const std::string& str) // Проверка строки на содержание только цифр
{
    for (unsigned char c : str)
    {
        if (!std::isdigit(c))
            return false;
    }
    return !str.empty();
}

bool SafeInput::containsLetters(const std::string& str) // Проверка строки на наличие букв
{
    for (unsigned char c : str)
    {
        if (isLetter(c))
            return true;
    }
    return false;
}

bool SafeInput::isValidTextField(const std::string& str) // Проверка валидности текстового поля
{
    return !str.empty() && containsLetters(str) && !containsOnlyDigits(str);
}

void SafeInput::validateTextField(const std::string& str, const std::string& fieldName) // Валидация текстового поля
{
    if (str.empty())
        throw InvalidProductDataException(fieldName, "cannot be empty");

    if (!containsLetters(str))
        throw InvalidProductDataException(fieldName, "must contain letters");

    if (containsOnlyDigits(str))
        throw InvalidProductDataException(fieldName, "cannot contain only digits");
}

void SafeInput::validateProductId(const std::string& id) // Валидация ID продукта
{
    if (id.empty())
        throw InvalidProductDataException("ID", "cannot be empty");

    if (id.length() < 3)
        throw InvalidProductDataException("ID", "must be at least 3 digits long");

    if (!containsOnlyDigits(id))
        throw InvalidProductDataException("ID", "must contain only digits");
}
