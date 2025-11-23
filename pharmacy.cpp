#include "pharmacy.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "Exception/safeinput.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/PharmacyExceptions/ProductNotFoundException.h"
#include "Exception/PharmacyExceptions/DuplicateProductException.h"

// Конструктор с name
Pharmacy::Pharmacy(const std::string& id, const std::string& name, const std::string& addr, const std::string& phone,
                   double rent)
    : id(id), name(name), address(addr), phoneNumber(phone), rentCost(rent)
{
}

// Конструктор по умолчанию
Pharmacy::Pharmacy()
    : id(""), name("Green Pharmacy"), address(""), phoneNumber(""), rentCost(0.0)
{
}

// Конструктор копирования
Pharmacy::Pharmacy(const Pharmacy& other)
    : id(other.id), name(other.name), address(other.address),
    phoneNumber(other.phoneNumber), rentCost(other.rentCost), storage(other.storage)
{
}

void Pharmacy::addToStorage(std::shared_ptr<MedicalProduct> product, int quantity)
{
    if (!product)
        throw InvalidProductDataException("product", "cannot be null");

    if (quantity <= 0)
        throw InvalidProductDataException("quantity", "must be positive");

    if (storage.contains(product->getId()))
        throw DuplicateProductException(product->getId());

    storage.addProduct(product, quantity);
}

void Pharmacy::removeFromStorage(const std::string& productId, int quantity)
{
    if (productId.empty())
        throw InvalidProductDataException("product ID", "cannot be empty");

    if (quantity <= 0)
        throw InvalidProductDataException("quantity", "must be positive");

    if (!storage.contains(productId))
        throw ProductNotFoundException(productId);

    storage.removeProduct(productId, quantity);
}

int Pharmacy::checkStock(const std::string& productId) const
{
    if (productId.empty())
        throw std::invalid_argument("Product ID cannot be empty");
    return storage.getQuantity(productId);         // Проверяем количество на складе
}

std::vector<std::shared_ptr<Medicine>> Pharmacy::findAvailableAnalogues(const std::string& medicineId) const
{
    if (medicineId.empty())
        throw std::invalid_argument("Medicine ID cannot be empty");

    std::vector<std::shared_ptr<Medicine>> result;

    // Находим лекарство по ID
    auto productIt = storage.items.find(medicineId);
    if (productIt == storage.items.end())
        return result;                              // Лекарство не найдено

    auto medicine = std::dynamic_pointer_cast<Medicine>(productIt->second.first);
    if (!medicine)
        return result;                              // Не является лекарством

    // Получаем все аналоги этого лекарства
    const auto& analogues = medicine->getAnalogues();

    // Фильтруем только те аналоги, которые есть в наличии
    for (const auto& analogue : analogues)
    {
        if (storage.contains(analogue->getId()) && storage.getQuantity(analogue->getId()) > 0)
            result.push_back(analogue);             // Добавляем доступный аналог
    }

    return result;
}

std::vector<std::pair<std::shared_ptr<MedicalProduct>, int>> Pharmacy::getAllProducts() const
{
    std::vector<std::pair<std::shared_ptr<MedicalProduct>, int>> result;

    for (const auto& item : storage.items)
        result.emplace_back(item.second.first, item.second.second);  // Продукт и количество

    return result;
}

Pharmacy& Pharmacy::operator=(const Pharmacy& other)
{
    if (this != &other)
    {
        id = other.id;                              // Копируем ID
        address = other.address;                    // Копируем адрес
        phoneNumber = other.phoneNumber;            // Копируем телефон
        rentCost = other.rentCost;                  // Копируем стоимость аренды
        storage = other.storage;                    // Копируем склад
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Pharmacy& pharmacy)
{
    os << pharmacy.id << ";"
       << pharmacy.address << ";"
       << pharmacy.phoneNumber << ";"
       << pharmacy.rentCost << ";"
       << pharmacy.storage.items.size();

    return os;
}

std::istream& operator>>(std::istream& is, Pharmacy& pharmacy)
{
    try
    {
        std::string line;
        std::getline(is, line);

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ';')) {
            tokens.push_back(token);
        }

        if (tokens.size() != 5) {
            throw InvalidProductDataException("pharmacy data", "invalid number of fields");
        }

        pharmacy.id = tokens[0];
        if (pharmacy.id.empty()) {
            throw InvalidProductDataException("Pharmacy ID", "cannot be empty");
        }

        pharmacy.address = tokens[1];
        if (pharmacy.address.empty()) {
            throw InvalidProductDataException("Address", "cannot be empty");
        }

        pharmacy.phoneNumber = tokens[2];
        if (pharmacy.phoneNumber.empty()) {
            throw InvalidProductDataException("Phone number", "cannot be empty");
        }

        std::istringstream rentStream(tokens[3]);
        pharmacy.rentCost = SafeInput::readPositiveDouble(rentStream, "Rent cost");

        // Количество продуктов (5-е поле) - только для информации

    }
    catch (const PharmacyException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("pharmacy data", e.what());
    }

    return is;
}
std::shared_ptr<MedicalProduct> Pharmacy::findProduct(const std::string& productNameOrId) const
{
    auto allProducts = getAllProducts();
    for (const auto& productPair : allProducts)
    {
        auto product = productPair.first;
        if (product->getId() == productNameOrId || product->getName() == productNameOrId)
            return product;
    }
    return nullptr;
}
