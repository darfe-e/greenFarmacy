#include "safeinput.h"
#include <algorithm>

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

    if (value == "yes" || value == "true" || value == "1")
    {
        return true;
    }
    else if (value == "no" || value == "false" || value == "0")
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

    // Проверка на допустимые символы (только буквы, цифры и дефисы)
    for (char c : id)
    {
        if (!std::isalnum(c) && c != '-')
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
