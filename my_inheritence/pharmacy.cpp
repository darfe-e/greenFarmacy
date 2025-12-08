#include "pharmacy.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "Exception/safeinput.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/PharmacyExceptions/ProductNotFoundException.h"
#include "Exception/PharmacyExceptions/DuplicateProductException.h"

Pharmacy::Pharmacy(const std::string& id, const std::string& name, const std::string& addr, const std::string& phone,
                   double rent)
    : id(id), name(name), address(addr), phoneNumber(phone), rentCost(rent)
{
}

Pharmacy::Pharmacy()
    : id(""), name("Green Pharmacy"), address(""), phoneNumber(""), rentCost(0.0)
{
}

Pharmacy::Pharmacy(const Pharmacy& other)
    : id(other.id), name(other.name), address(other.address),
    phoneNumber(other.phoneNumber), rentCost(other.rentCost), storage(other.storage)
{
}

void Pharmacy::addToStorage(std::shared_ptr<MedicalProduct> product, int quantity)
{
    if (!product)                                                                   // Проверка нулевого указателя
        throw InvalidProductDataException("product", "cannot be null");

    if (quantity <= 0)                                                              // Проверка положительности количества
        throw InvalidProductDataException("quantity", "must be positive");

    if (storage.contains(product->getId()))                                         // Проверка дубликата продукта
        throw DuplicateProductException(product->getId());

    storage.addProduct(product, quantity);                                          // Добавление продукта на склад
}

void Pharmacy::removeFromStorage(const std::string& productId, int quantity)
{
    if (productId.empty())                                                          // Проверка пустого ID
        throw InvalidProductDataException("product ID", "cannot be empty");

    if (quantity <= 0)                                                              // Проверка положительности количества
        throw InvalidProductDataException("quantity", "must be positive");

    if (!storage.contains(productId))                                               // Проверка существования продукта
        throw ProductNotFoundException(productId);

    storage.removeProduct(productId, quantity);                                     // Удаление продукта со склада
}

int Pharmacy::checkStock(const std::string& productId) const
{
    if (productId.empty())                                                          // Проверка пустого ID
        throw std::invalid_argument("Product ID cannot be empty");
    return storage.getQuantity(productId);                                          // Возврат количества на складе
}

std::vector<std::shared_ptr<Medicine>> Pharmacy::findAvailableAnalogues(const std::string& medicineId) const
{
    if (medicineId.empty())                                                         // Проверка пустого ID лекарства
        throw std::invalid_argument("Medicine ID cannot be empty");

    std::vector<std::shared_ptr<Medicine>> result;                                  // Вектор для результатов

    auto productIt = storage.items.find(medicineId);                                // Поиск лекарства в хранилище
    if (productIt == storage.items.end())                                           // Если лекарство не найдено
        return result;

    auto medicine = std::dynamic_pointer_cast<Medicine>(productIt->second.first);   // Приведение к типу Medicine
    if (!medicine)                                                                  // Если продукт не является лекарством
        return result;

    const auto& analogues = medicine->getAnalogues();                               // Получение всех аналогов лекарства

    for (const auto& analogue : analogues)                                          // Проход по всем аналогам
    {
        if (storage.contains(analogue->getId()) && storage.getQuantity(analogue->getId()) > 0)  // Проверка наличия
            result.push_back(analogue);                                             // Добавление доступного аналога
    }

    return result;                                                                  // Возврат списка доступных аналогов
}

std::vector<std::pair<std::shared_ptr<MedicalProduct>, int>> Pharmacy::getAllProducts() const
{
    std::vector<std::pair<std::shared_ptr<MedicalProduct>, int>> result;            // Вектор для всех продуктов

    for (const auto& item : storage.items)                                          // Проход по всем элементам хранилища
        result.emplace_back(item.second.first, item.second.second);                 // Добавление пары продукт-количество

    return result;                                                                  // Возврат списка всех продуктов
}

Pharmacy& Pharmacy::operator=(const Pharmacy& other)
{
    if (this != &other)                                                             // Проверка самоприсваивания
    {
        id = other.id;                                                              // Копирование ID
        name = other.name;                                                          // Копирование названия
        address = other.address;                                                    // Копирование адреса
        phoneNumber = other.phoneNumber;                                            // Копирование телефона
        rentCost = other.rentCost;                                                  // Копирование стоимости аренды
        storage = other.storage;                                                    // Копирование склада
    }
    return *this;                                                                   // Возврат текущего объекта
}

std::ostream& operator<<(std::ostream& os, const Pharmacy& pharmacy)
{
    os << pharmacy.id << ";"                                                        // Вывод ID аптеки
       << pharmacy.name << ";"                                                      // Вывод названия аптеки
       << pharmacy.address << ";"                                                   // Вывод адреса
       << pharmacy.phoneNumber << ";"                                               // Вывод телефона
       << pharmacy.rentCost << ";"                                                  // Вывод стоимости аренды
       << pharmacy.storage.items.size();                                            // Вывод количества продуктов

    return os;                                                                      // Возврат потока
}

std::istream& operator>>(std::istream& is, Pharmacy& pharmacy)
{
    try
    {
        std::string line;
        std::getline(is, line);                                                     // Чтение строки из потока

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ';'))                                       // Разделение строки по ';'
            tokens.push_back(token);                                                // Сохранение токена

        if (tokens.size() != 5)                                                     // Проверка количества полей
            throw InvalidProductDataException("pharmacy data", "invalid number of fields: expected 5, got " + std::to_string(tokens.size()));

        pharmacy.id = tokens[0];                                                    // Чтение ID аптеки
        if (pharmacy.id.empty())                                                    // Проверка непустого ID
            throw InvalidProductDataException("Pharmacy ID", "cannot be empty");

        pharmacy.name = tokens[1];                                                  // Чтение названия аптеки
        if (pharmacy.name.empty())                                                  // Если название пустое
            pharmacy.name = "Аптека №" + pharmacy.id;                              // Запасное название

        pharmacy.address = tokens[2];                                               // Чтение адреса
        if (pharmacy.address.empty())                                               // Проверка непустого адреса
            throw InvalidProductDataException("Address", "cannot be empty");

        pharmacy.phoneNumber = tokens[3];                                           // Чтение телефона
        if (pharmacy.phoneNumber.empty())                                           // Проверка непустого телефона
            throw InvalidProductDataException("Phone number", "cannot be empty");

        std::istringstream rentStream(tokens[4]);                                   // Чтение стоимости аренды
        pharmacy.rentCost = SafeInput::readPositiveDouble(rentStream, "Rent cost"); // Преобразование в double
    }
    catch (const PharmacyException&)                                                // Переброс исключений PharmacyException
    {
        throw;
    }
    catch (const std::exception& e)                                                 // Обработка других исключений
    {
        throw InvalidProductDataException("pharmacy data", e.what());
    }

    return is;                                                                      // Возврат потока
}

std::shared_ptr<MedicalProduct> Pharmacy::findProduct(const std::string& productNameOrId) const
{
    auto allProducts = getAllProducts();                                            // Получение всех продуктов
    for (const auto& productPair : allProducts)                                     // Проход по всем продуктам
    {
        auto product = productPair.first;
        if (product->getId() == productNameOrId || product->getName() == productNameOrId)  // Сравнение по ID или имени
            return product;                                                        // Возврат найденного продукта
    }
    return nullptr;                                                                 // Если продукт не найден
}
