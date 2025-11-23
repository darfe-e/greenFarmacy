#include "medicalproduct.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/PharmacyExceptions/ExpiredProductException.h"
#include "Exception/PharmacyExceptions/InvalidPriceException.h"
#include "Exception/PharmacyExceptions/InvalidProductIdException.h"
#include "safedate.h"
#include "Exception/safeinput.h"

MedicalProduct::MedicalProduct()
    : id(""), name(""), basePrice(0.0), expirationDate(SafeDate(2025, 12, 31)), manufacturerCountry("")
{
}

MedicalProduct::MedicalProduct(std::string id, std::string name, double basePrice,
                               SafeDate expDate, std::string country)
    : id(std::move(id)), name(std::move(name)), basePrice(basePrice),
    expirationDate(expDate), manufacturerCountry(std::move(country))
{
    if (this->id.empty()) throw InvalidProductDataException("id", "cannot be empty");
    if (this->name.empty()) throw InvalidProductDataException("name", "cannot be empty");
    if (this->basePrice < 0) throw InvalidProductDataException("price", "cannot be negative");
    if (this->manufacturerCountry.empty()) throw InvalidProductDataException("manufacturer country", "cannot be empty");

    try
    {
        if (expDate.isExpired()) throw ExpiredProductException(this->id, expDate);
    }
    catch (const ExpiredProductException&)
    {
        throw; // Перебрасываем как есть
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("expiration date", e.what());
    }
}

MedicalProduct::MedicalProduct(const MedicalProduct& other)
    : id(other.id), name(other.name), basePrice(other.basePrice),
    expirationDate(other.expirationDate), manufacturerCountry(other.manufacturerCountry)
{
}

std::ostream& operator<<(std::ostream& os, const MedicalProduct& prod)
{
    SafeDate safeDate(prod.expirationDate);  // Создаем объект SafeDate

    os << prod.id << ";"
       << prod.name << ";"
       << prod.basePrice << ";"
       << safeDate.toString() << ";"
       << prod.manufacturerCountry;

    return os;
}

std::istream& operator>>(std::istream& is, MedicalProduct& prod)
{
    try
    {
        std::string line;

        // Читаем всю строку
        std::getline(is, line);

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        // Разбиваем строку по разделителю ';'
        while (std::getline(iss, token, ';')) {
            tokens.push_back(token);
        }

        // Проверяем, что получили все 5 полей
        if (tokens.size() != 5) {
            throw InvalidProductDataException("input", "invalid number of fields");
        }

        // Заполняем поля продукта
        prod.id = tokens[0];
        prod.name = tokens[1];

        // Преобразуем строки в числа с помощью SafeInput
        std::istringstream priceStream(tokens[2]);
        prod.basePrice = SafeInput::readPositiveDouble(priceStream, "Price");

        prod.expirationDate = SafeDate::fromString(tokens[3]);
        prod.manufacturerCountry = tokens[4];

        // Проверка срока годности
        if (prod.expirationDate.isExpired())
        {
            throw ExpiredProductException(prod.id, prod.expirationDate);
        }

        // Дополнительная валидация ID (как в readProductId)
        if (prod.id.length() < 3)
        {
            throw InvalidProductIdException(prod.id);
        }

        for (char c : prod.id)
        {
            if (!std::isalnum(c) && c != '-')
            {
                throw InvalidProductIdException(prod.id);
            }
        }

        // Проверка непустоты строковых полей
        if (prod.name.empty()) {
            throw InvalidProductDataException("Product name", "cannot be empty");
        }

        if (prod.manufacturerCountry.empty()) {
            throw InvalidProductDataException("Manufacturer country", "cannot be empty");
        }
    }
    catch (const ExpiredProductException&)
    {
        throw; // Перебрасываем как есть
    }
    catch (const PharmacyException&)
    {
        throw; // Перебрасываем кастомные исключения
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("input data", e.what());
    }

    return is;
}
MedicalProduct& MedicalProduct::operator=(const MedicalProduct& other)
{
    if (this != &other)
    {
        id = other.id;
        name = other.name;
        basePrice = other.basePrice;
        expirationDate = other.expirationDate;
        manufacturerCountry = other.manufacturerCountry;
    }
    return *this;
}
