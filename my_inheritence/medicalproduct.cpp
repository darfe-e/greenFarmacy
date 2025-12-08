#include "medicalproduct.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/PharmacyExceptions/ExpiredProductException.h"
#include "safedate.h"
#include "Exception/safeinput.h"

MedicalProduct::MedicalProduct()                                              // Конструктор по умолчанию
    : id(""), name(""), basePrice(0.0), expirationDate(SafeDate(2025, 12, 31)), manufacturerCountry("")
{
}

MedicalProduct::MedicalProduct(std::string id, std::string name, double basePrice, // Конструктор с параметрами
                               SafeDate expDate, std::string country)
    : id(std::move(id)), name(std::move(name)), basePrice(basePrice),
    expirationDate(expDate), manufacturerCountry(std::move(country))
{
    if (this->id.empty()) throw InvalidProductDataException("id", "cannot be empty");
    if (this->name.empty()) throw InvalidProductDataException("name", "cannot be empty");
    if (this->basePrice < 0) throw InvalidProductDataException("price", "cannot be negative");
    if (this->manufacturerCountry.empty()) throw InvalidProductDataException("manufacturer country", "cannot be empty");
}

MedicalProduct::MedicalProduct(const MedicalProduct& other)                  // Конструктор копирования
    : id(other.id), name(other.name), basePrice(other.basePrice),
    expirationDate(other.expirationDate), manufacturerCountry(other.manufacturerCountry)
{
}

std::ostream& operator<<(std::ostream& os, const MedicalProduct& prod)        // Вывод в поток
{
    SafeDate safeDate(prod.expirationDate);

    os << prod.id << ";"
       << prod.name << ";"
       << prod.basePrice << ";"
       << safeDate.toString() << ";"
       << prod.manufacturerCountry;

    return os;
}

std::istream& operator>>(std::istream& is, MedicalProduct& prod)              // Ввод из потока
{
    try
    {
        std::string line;
        std::getline(is, line);

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ';'))
            tokens.push_back(token);

        if (tokens.size() != 5)
            throw InvalidProductDataException("input", "invalid number of fields");

        prod.id = tokens[0];
        prod.name = tokens[1];

        std::istringstream priceStream(tokens[2]);
        prod.basePrice = SafeInput::readPositiveDouble(priceStream, "Price");

        prod.expirationDate = SafeDate::fromString(tokens[3]);
        prod.manufacturerCountry = tokens[4];

        if (prod.expirationDate.isExpired())
            throw ExpiredProductException(prod.id, prod.expirationDate);

        if (prod.id.length() < 3)
            throw InvalidProductDataException(prod.id, "Неверное значение id!");

        for (char c : prod.id)
        {
            if (!std::isalnum(c) && c != '-')
                throw InvalidProductDataException(prod.id, "Неверное значение id!");
        }

        if (prod.name.empty())
            throw InvalidProductDataException("Product name", "cannot be empty");

        if (prod.manufacturerCountry.empty())
            throw InvalidProductDataException("Manufacturer country", "cannot be empty");
    }
    catch (const ExpiredProductException&)
    {
        throw;
    }
    catch (const PharmacyException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("input data", e.what());
    }

    return is;
}

MedicalProduct& MedicalProduct::operator=(const MedicalProduct& other)        // Оператор присваивания
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
