#include "pharmacymanager.h"
#include <algorithm>
#include <iostream>
#include "tablet.h"
#include <fstream>
#include <sstream>

PharmacyManager::PharmacyManager()
    : pharmaciesTree(PharmacyComparator())                        // Инициализация дерева с компаратором для сортировки
{
}

void PharmacyManager::addProduct(std::shared_ptr<MedicalProduct> product)
{
    if (!product)                                                                   // Проверка нулевого указателя
        throw InvalidProductDataException("product", "cannot be null");

    if (productsCatalog.find(product->getId()) != productsCatalog.end())            // Проверка существования продукта
        throw DuplicateProductException(product->getId());

    productsCatalog[product->getId()] = product;                                    // Добавление продукта в каталог
}

void PharmacyManager::removeProduct(const std::string& productId)
{
    if (productId.empty())                                                          // Проверка пустого ID
        throw InvalidProductDataException("product ID", "cannot be empty");

    auto it = productsCatalog.find(productId);                                      // Поиск продукта в каталоге
    if (it == productsCatalog.end())                                                // Если продукт не найден
        throw ProductNotFoundException(productId);

    productsCatalog.erase(it);                                                      // Удаление продукта из каталога
}

std::shared_ptr<MedicalProduct> PharmacyManager::getProduct(const std::string& productId) const
{
    if (productId.empty())                                                          // Проверка пустого ID
        throw InvalidProductDataException("product ID", "cannot be empty");

    auto it = productsCatalog.find(productId);                                      // Поиск продукта в каталоге
    if (it == productsCatalog.end())                                                // Если продукт не найден
        throw ProductNotFoundException(productId);

    return it->second;                                                              // Возврат найденного продукта
}

void PharmacyManager::addPharmacy(std::shared_ptr<Pharmacy> pharmacy)
{
    if (!pharmacy)                                                                  // Проверка нулевого указателя
        throw InvalidProductDataException("pharmacy", "cannot be null");

    auto existing = findPharmacyInTree(pharmacy->getId());                          // Поиск существующей аптеки
    if (existing)                                                                   // Если аптека уже существует
        throw DuplicateProductException("Pharmacy with ID: " + pharmacy->getId());

    pharmaciesTree.push(pharmacy);                                                  // Добавление аптеки в дерево
}

void PharmacyManager::removePharmacy(const std::string& pharmacyId)
{
    if (pharmacyId.empty())                                                         // Проверка пустого ID
        throw InvalidProductDataException("pharmacy ID", "cannot be empty");

    auto pharmacy = findPharmacyInTree(pharmacyId);                                 // Поиск аптеки в дереве
    if (!pharmacy)                                                                  // Если аптека не найдена
        throw ProductNotFoundException("Pharmacy with ID: " + pharmacyId);

    pharmaciesTree.remove(pharmacy);                                                // Удаление аптеки из дерева
}

std::shared_ptr<Pharmacy> PharmacyManager::getPharmacy(const std::string& pharmacyId) const
{
    if (pharmacyId.empty())                                                         // Проверка пустого ID
        throw InvalidProductDataException("pharmacy ID", "cannot be empty");

    return findPharmacyInTree(pharmacyId);                                          // Поиск и возврат аптеки
}

void PharmacyManager::addOperation(std::shared_ptr<InventoryOperation> operation)
{
    if (!operation)                                                                 // Проверка нулевого указателя
        throw InvalidProductDataException("operation", "cannot be null");

    operations.push_back(operation);                                                // Добавление операции в список
}

std::vector<std::shared_ptr<Supply>> PharmacyManager::getSupplyOperations() const
{
    std::vector<std::shared_ptr<Supply>> supplies;                                  // Вектор для операций поставки
    for (const auto& op : operations)                                               // Проход по всем операциям
        if (auto supply = std::dynamic_pointer_cast<Supply>(op))                    // Приведение к типу Supply
            supplies.push_back(supply);                                             // Добавление в результат

    return supplies;                                                                // Возврат списка поставок
}

std::vector<std::shared_ptr<Return>> PharmacyManager::getReturnOperations() const
{
    std::vector<std::shared_ptr<Return>> returns;                                   // Вектор для операций возврата
    for (const auto& op : operations)                                               // Проход по всем операциям
        if (auto returnOp = std::dynamic_pointer_cast<Return>(op))                  // Приведение к типу Return
            returns.push_back(returnOp);                                            // Добавление в результат

    return returns;                                                                 // Возврат списка возвратов
}

std::vector<std::shared_ptr<WriteOff>> PharmacyManager::getWriteOffOperations() const
{
    std::vector<std::shared_ptr<WriteOff>> writeOffs;                               // Вектор для операций списания
    for (const auto& op : operations)                                               // Проход по всем операциям
        if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(op))                // Приведение к типу WriteOff
            writeOffs.push_back(writeOff);                                          // Добавление в результат

    return writeOffs;                                                               // Возврат списка списаний
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsByCountry(const std::string& country) const
{
    if (country.empty())                                                            // Проверка пустой строки страны
        throw InvalidProductDataException("country", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;                            // Вектор для результатов
    for (const auto& pair : productsCatalog)                                        // Проход по всем продуктам
        if (pair.second->getManufacturerCountry().find(country) != std::string::npos)// Поиск страны в названии страны
            result.push_back(pair.second);                                          // Добавление продукта в результат

    return result;                                                                  // Возврат найденных продуктов
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsBySubstance(const std::string& substance) const
{
    if (substance.empty())                                                          // Проверка пустой строки вещества
        throw InvalidProductDataException("substance", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;                            // Вектор для результатов
    for (const auto& pair : productsCatalog)                                        // Проход по всем продуктам
        if (auto medicine = std::dynamic_pointer_cast<Medicine>(pair.second))       // Приведение к типу Medicine
            if (medicine->getActiveSubstance().find(substance) != std::string::npos)// Поиск вещества в названии
                result.push_back(medicine);                                         // Добавление лекарства в результат

    return result;                                                                  // Возврат найденных лекарств
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProducts(const std::string& searchTerm) const
{
    if (searchTerm.empty())                                                         // Проверка пустой строки поиска
        throw InvalidProductDataException("search term", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;                            // Вектор для результатов
    for (const auto& pair : productsCatalog)                                        // Проход по всем продуктам
    {
        auto product = pair.second;
        if (product->getName().find(searchTerm) != std::string::npos ||             // Поиск в имени
            product->getId().find(searchTerm) != std::string::npos ||               // Поиск в ID
            product->getManufacturerCountry().find(searchTerm) != std::string::npos)// Поиск в стране
            result.push_back(product);                                              // Добавление продукта в результат
        else if (auto medicine = std::dynamic_pointer_cast<Medicine>(product))      // Если продукт - лекарство
            if (medicine->getActiveSubstance().find(searchTerm) != std::string::npos)// Поиск в действующем веществе
                result.push_back(medicine);                                         // Добавление лекарства в результат
    }
    return result;                                                                  // Возврат найденных продуктов
}

std::map<std::string, int> PharmacyManager::getProductAvailability(const std::string& productId) const
{
    if (productId.empty())                                                          // Проверка пустого ID
        throw InvalidProductDataException("product ID", "cannot be empty");

    std::map<std::string, int> availability;                                        // Карта доступности продукта

    binaryTree<std::shared_ptr<Pharmacy>> tempTree = pharmaciesTree;                // Создание временной копии дерева

    for (auto it = tempTree.begin(); it != tempTree.end(); ++it)                    // Обход всех аптек
    {
        auto pharmacy = *it;
        int quantity = pharmacy->checkStock(productId);                             // Проверка наличия продукта
        if (quantity > 0)                                                           // Если продукт есть в наличии
            availability[pharmacy->getId()] = quantity;                             // Добавление в карту доступности
    }

    return availability;                                                            // Возврат карты доступности
}

std::vector<std::pair<std::string, std::string>> PharmacyManager::findProductInPharmacies(const std::string& productNameOrId) const
{
    if (productNameOrId.empty())                                                    // Проверка пустой строки поиска
        throw InvalidProductDataException("product name or ID", "cannot be empty");

    std::vector<std::pair<std::string, std::string>> result;                        // Вектор для результатов

    for (auto it = pharmaciesTree.begin(); it != pharmaciesTree.end(); ++it)        // Обход всех аптек
    {
        auto pharmacy = *it;
        auto product = pharmacy->findProduct(productNameOrId);                      // Поиск продукта в аптеке
        if (product)                                                                // Если продукт найден
            result.emplace_back(pharmacy->getId(), pharmacy->getName());            // Добавление аптеки в результат
    }

    return result;                                                                  // Возврат списка аптек с продуктом
}

std::vector<std::shared_ptr<Medicine>> PharmacyManager::getAnalogues(const std::string& productId) const
{
    if (productId.empty())                                                          // Проверка пустого ID
        throw InvalidProductDataException("product ID", "cannot be empty");

    std::vector<std::shared_ptr<Medicine>> analogues;                               // Вектор для аналогов
    auto original = std::dynamic_pointer_cast<Medicine>(getProduct(productId));     // Получение оригинального лекарства

    if (!original)                                                                  // Если продукт не является лекарством
        throw InvalidProductDataException("product", "is not a medicine or not found");

    for (const auto& productPair : productsCatalog)                                 // Проход по всем продуктам
    {
        auto medicine = std::dynamic_pointer_cast<Medicine>(productPair.second);
        if (medicine && medicine->getId() != productId &&                           // Не сам продукт
            medicine->getActiveSubstance() == original->getActiveSubstance())       // Одинаковое действующее вещество
            analogues.push_back(medicine);                                          // Добавление аналога
    }

    return analogues;                                                               // Возврат списка аналогов
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsByName(const std::string& name) const
{
    if (name.empty())                                                               // Проверка пустого имени
        throw InvalidProductDataException("name", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;                            // Вектор для результатов
    for (const auto& pair : productsCatalog)                                        // Проход по всем продуктам
    {
        auto product = pair.second;
        if (product->getName().find(name) != std::string::npos ||                    // Поиск в имени
            product->getId().find(name) != std::string::npos ||                      // Поиск в ID
            product->getManufacturerCountry().find(name) != std::string::npos)       // Поиск в стране
            result.push_back(product);                                              // Добавление продукта в результат
    }
    return result;                                                                  // Возврат найденных продуктов
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::getAllProducts() const
{
    std::vector<std::shared_ptr<MedicalProduct>> result;                            // Вектор для всех продуктов
    for (const auto& pair : productsCatalog)                                        // Проход по всем продуктам
        result.push_back(pair.second);                                              // Добавление продукта в результат

    return result;                                                                  // Возврат всех продуктов
}

void PharmacyManager::displaySupplyInfo() const
{
    std::cout << "=== Supply Operations ===\n";
    for (const auto& operation : operations)                                        // Проход по всем операциям
    {
        auto supply = std::dynamic_pointer_cast<Supply>(operation);                 // Приведение к типу Supply
        if (supply)                                                                 // Если операция - поставка
            std::cout << *supply << "---\n";                                        // Вывод информации о поставке
    }
}

void PharmacyManager::displayReturnInfo() const
{
    std::cout << "=== Return Operations ===\n";
    for (const auto& operation : operations)                                        // Проход по всем операциям
    {
        auto returnOp = std::dynamic_pointer_cast<Return>(operation);               // Приведение к типу Return
        if (returnOp)                                                               // Если операция - возврат
            std::cout << *returnOp << "---\n";                                      // Вывод информации о возврате
    }
}

void PharmacyManager::displayWriteOffInfo() const
{
    std::cout << "=== Write-off Operations ===\n";
    for (const auto& operation : operations)                                        // Проход по всем операциям
    {
        auto writeOff = std::dynamic_pointer_cast<WriteOff>(operation);             // Приведение к типу WriteOff
        if (writeOff)                                                               // Если операция - списание
            std::cout << *writeOff << "---\n";                                      // Вывод информации о списании
    }
}

bool PharmacyManager::updateProduct(std::shared_ptr<MedicalProduct> updatedProduct)
{
    std::string id = updatedProduct->getId();

    auto it = productsCatalog.find(id);                                             // Поиск продукта по ID
    if (it != productsCatalog.end())                                                // Если продукт найден
    {
        it->second = updatedProduct;                                                // Обновление продукта
        return true;                                                                // Возврат успеха
    }

    return false;                                                                   // Продукт не найден
}

void PharmacyManager::loadSuppliesFromFile(const std::string& filename)
{
    try
    {
        std::ifstream file(filename);                                               // Открытие файла
        if (!file.is_open())                                                        // Если файл не открылся
            throw std::runtime_error("Cannot open supplies file");

        std::string line;
        while (std::getline(file, line))                                            // Чтение файла построчно
        {
            if (!line.empty())                                                      // Пропуск пустых строк
            {
                std::istringstream iss(line);
                std::string type;
                std::getline(iss, type, ';');                                       // Чтение типа операции

                if (type == "SUPPLY")                                               // Если операция - поставка
                {
                    Supply supply;
                    iss >> supply;                                                  // Десериализация поставки
                    addOperation(std::make_shared<Supply>(supply));                 // Добавление операции
                }
            }
        }

        file.close();                                                               // Закрытие файла
    }
    catch (const std::exception& e)                                                 // Обработка исключений
    {
        throw std::runtime_error("Error loading supplies: " + std::string(e.what()));
    }
}

std::vector<std::shared_ptr<InventoryOperation>> PharmacyManager::getAllOperations() const
{
    return operations;                                                              // Возврат всех операций
}

void PharmacyManager::clearAll()
{
    productsCatalog.clear();                                                        // Очистка каталога продуктов
    operations.clear();                                                             // Очистка списка операций
    pharmaciesTree.clear();                                                         // Очистка дерева аптек
}

std::vector<std::shared_ptr<Pharmacy>> PharmacyManager::getAllPharmacies() const
{
    std::vector<std::shared_ptr<Pharmacy>> result;                                  // Вектор для всех аптек

    for (auto it = pharmaciesTree.begin(); it != pharmaciesTree.end(); ++it)        // Обход дерева аптек
        result.push_back(*it);                                                      // Добавление аптеки в результат

    return result;                                                                  // Возврат всех аптек
}

std::shared_ptr<Pharmacy> PharmacyManager::findPharmacyInTree(const std::string& pharmacyId) const
{
    auto it = pharmaciesTree.find_if([&pharmacyId](const std::shared_ptr<Pharmacy>& pharmacy) {
        return pharmacy->getId() == pharmacyId;                                     // Поиск по ID аптеки
    });

    if (it != pharmaciesTree.end())                                                 // Если аптека найдена
        return *it;                                                                 // Возврат найденной аптеки

    return nullptr;                                                                 // Аптека не найдена
}
