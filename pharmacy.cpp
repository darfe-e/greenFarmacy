#include "pharmacy.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>

Pharmacy::Pharmacy(const std::string& id, const std::string& addr, const std::string& phone,
                   double rent)
    : id(id)
    , address(addr)
    , phoneNumber(phone)
    , rentCost(rent)
    , storage()
{
    if (id.empty())
        throw std::invalid_argument("Pharmacy ID cannot be empty");
    if (address.empty())
        throw std::invalid_argument("Address cannot be empty");
    if (phoneNumber.empty())
        throw std::invalid_argument("Phone number cannot be empty");
    if (rentCost < 0)
        throw std::invalid_argument("Rent cost cannot be negative");
}

Pharmacy::Pharmacy()
    : id("")
    , address("")
    , phoneNumber("")
    , rentCost(0.0)
    , storage()
{
}

Pharmacy::Pharmacy(const Pharmacy& other)
    : id(other.id)
    , address(other.address)
    , phoneNumber(other.phoneNumber)
    , rentCost(other.rentCost)
    , storage(other.storage)
{
}

void Pharmacy::addToStorage(std::shared_ptr<MedicalProduct> product, int quantity)
{
    if (!product)
        throw std::invalid_argument("Product cannot be null");
    if (quantity <= 0)
        throw std::invalid_argument("Quantity must be positive");
    storage.addProduct(product, quantity);         // Добавляем продукт на склад
}

void Pharmacy::removeFromStorage(const std::string& productId, int quantity)
{
    if (productId.empty())
        throw std::invalid_argument("Product ID cannot be empty");
    if (quantity <= 0)
        throw std::invalid_argument("Quantity must be positive");
    storage.removeProduct(productId, quantity);    // Удаляем продукт со склада
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
    os << "Pharmacy ID: " << pharmacy.id << "\n"
       << "Address: " << pharmacy.address << "\n"
       << "Phone: " << pharmacy.phoneNumber << "\n"
       << "Rent Cost: " << pharmacy.rentCost << "\n"
       << "Products in storage: " << pharmacy.storage.items.size() << "\n";

    // Выводим информацию о продуктах на складе
    for (const auto& item : pharmacy.storage.items)
    {
        os << "  - " << item.second.first->getName()
           << " (" << item.second.first->getId() << "): "
           << item.second.second << " units\n";
    }

    return os;
}

std::istream& operator>>(std::istream& is, Pharmacy& pharmacy)
{
    try
    {
        std::string temp;

        std::getline(is, temp);                    // "Pharmacy ID: "
        std::getline(is, pharmacy.id);             // Читаем ID аптеки

        std::getline(is, temp);                    // "Address: "
        std::getline(is, pharmacy.address);        // Читаем адрес

        std::getline(is, temp);                    // "Phone: "
        std::getline(is, pharmacy.phoneNumber);    // Читаем телефон

        std::getline(is, temp);                    // "Rent Cost: "
        std::getline(is, temp);
        pharmacy.rentCost = std::stod(temp);       // Читаем стоимость аренды

        std::getline(is, temp);                    // "Products in storage: X"
        // Пропускаем информацию о продуктах (они загружаются отдельно)

    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading Pharmacy data: " + std::string(e.what()));
    }

    return is;
}
