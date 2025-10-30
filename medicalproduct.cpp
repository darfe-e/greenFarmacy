#include "medicalproduct.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/PharmacyExceptions/ExpiredProductException.h"
#include "safedate.h"

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

MedicalProduct::MedicalProduct(const MedicalProduct& other): id(other.id),  name(other.name),
                        basePrice(other.basePrice), expirationDate(other.expirationDate),
                        manufacturerCountry(other.manufacturerCountry){}


std::ostream& operator<<(std::ostream& os, const MedicalProduct& prod)
{
    SafeDate safeDate(prod.expirationDate);  // Создаем объект SafeDate

    os << "Medicine information:\n"
       << "id: " << prod.id << "\n"
       << "Name: " << prod.name << "\n"
       << "Base price: " << prod.basePrice << "\n"
       << "Expiration date: " << safeDate.toString() << "\n"
       << "Manufacturer country: " << prod.manufacturerCountry;

    return os;
}

MedicalProduct& MedicalProduct::operator=(const MedicalProduct& other)
{
    if (this != &other) {  // Проверка самоприсваивания
        id = other.id;
        name = other.name;
        basePrice = other.basePrice;
        expirationDate = other.expirationDate;
        manufacturerCountry = other.manufacturerCountry;
    }
    return *this;  // Возврат ссылки на текущий объект
}
