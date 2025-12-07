#include "pharmacymanager.h"
#include <algorithm>
#include <iostream>
#include "tablet.h"
#include <fstream>
#include <sstream>

PharmacyManager::PharmacyManager()
    : pharmaciesTree(PharmacyComparator())  // Инициализируем дерево с компаратором
{
}

void PharmacyManager::addProduct(std::shared_ptr<MedicalProduct> product)
{
    if (!product)
        throw InvalidProductDataException("product", "cannot be null");

    if (productsCatalog.find(product->getId()) != productsCatalog.end())
        throw DuplicateProductException(product->getId());

    productsCatalog[product->getId()] = product;
}

void PharmacyManager::removeProduct(const std::string& productId)
{
    if (productId.empty())
        throw InvalidProductDataException("product ID", "cannot be empty");

    auto it = productsCatalog.find(productId);
    if (it == productsCatalog.end())
        throw ProductNotFoundException(productId);

    productsCatalog.erase(it);
}

std::shared_ptr<MedicalProduct> PharmacyManager::getProduct(const std::string& productId) const
{
    if (productId.empty())
        throw InvalidProductDataException("product ID", "cannot be empty");

    auto it = productsCatalog.find(productId);
    if (it == productsCatalog.end())
        throw ProductNotFoundException(productId);

    return it->second;
}

// Управление аптеками через бинарное дерево
void PharmacyManager::addPharmacy(std::shared_ptr<Pharmacy> pharmacy)
{
    if (!pharmacy)
        throw InvalidProductDataException("pharmacy", "cannot be null");

    // Проверяем, нет ли уже аптеки с таким ID
    auto existing = findPharmacyInTree(pharmacy->getId());
    if (existing)
        throw DuplicateProductException("Pharmacy with ID: " + pharmacy->getId());

    // Добавляем в дерево
    pharmaciesTree.push(pharmacy);
}

void PharmacyManager::removePharmacy(const std::string& pharmacyId)
{
    if (pharmacyId.empty())
        throw InvalidProductDataException("pharmacy ID", "cannot be empty");

    // Находим аптеку в дереве
    auto pharmacy = findPharmacyInTree(pharmacyId);
    if (!pharmacy)
        throw ProductNotFoundException("Pharmacy with ID: " + pharmacyId);

    // Удаляем из дерева
    pharmaciesTree.remove(pharmacy);
}

std::shared_ptr<Pharmacy> PharmacyManager::getPharmacy(const std::string& pharmacyId) const
{
    if (pharmacyId.empty())
        throw InvalidProductDataException("pharmacy ID", "cannot be empty");

    return findPharmacyInTree(pharmacyId);
}

void PharmacyManager::addOperation(std::shared_ptr<InventoryOperation> operation)
{
    if (!operation)
        throw InvalidProductDataException("operation", "cannot be null");

    operations.push_back(operation);
}

// Реализуем методы для получения операций по типам
std::vector<std::shared_ptr<Supply>> PharmacyManager::getSupplyOperations() const {
    std::vector<std::shared_ptr<Supply>> supplies;
    for (const auto& op : operations) {
        if (auto supply = std::dynamic_pointer_cast<Supply>(op)) {
            supplies.push_back(supply);
        }
    }
    return supplies;
}

std::vector<std::shared_ptr<Return>> PharmacyManager::getReturnOperations() const {
    std::vector<std::shared_ptr<Return>> returns;
    for (const auto& op : operations) {
        if (auto returnOp = std::dynamic_pointer_cast<Return>(op)) {
            returns.push_back(returnOp);
        }
    }
    return returns;
}

std::vector<std::shared_ptr<WriteOff>> PharmacyManager::getWriteOffOperations() const {
    std::vector<std::shared_ptr<WriteOff>> writeOffs;
    for (const auto& op : operations) {
        if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(op)) {
            writeOffs.push_back(writeOff);
        }
    }
    return writeOffs;
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsByCountry(const std::string& country) const
{
    if (country.empty())
        throw InvalidProductDataException("country", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
        if (pair.second->getManufacturerCountry().find(country) != std::string::npos)
            result.push_back(pair.second);

    return result;
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsBySubstance(const std::string& substance) const
{
    if (substance.empty())
        throw InvalidProductDataException("substance", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
        if (auto medicine = std::dynamic_pointer_cast<Medicine>(pair.second))
            if (medicine->getActiveSubstance().find(substance) != std::string::npos)
                result.push_back(medicine);

    return result;
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProducts(const std::string& searchTerm) const
{
    if (searchTerm.empty())
        throw InvalidProductDataException("search term", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
    {
        auto product = pair.second;
        if (product->getName().find(searchTerm) != std::string::npos ||
            product->getId().find(searchTerm) != std::string::npos ||
            product->getManufacturerCountry().find(searchTerm) != std::string::npos)
            result.push_back(product);
        else if (auto medicine = std::dynamic_pointer_cast<Medicine>(product))
            if (medicine->getActiveSubstance().find(searchTerm) != std::string::npos)
                result.push_back(medicine);
    }
    return result;
}

std::map<std::string, int> PharmacyManager::getProductAvailability(const std::string& productId) const
{
    if (productId.empty())
        throw InvalidProductDataException("product ID", "cannot be empty");

    std::map<std::string, int> availability;

    // Создаем копию дерева для обхода (неконстантную)
    binaryTree<std::shared_ptr<Pharmacy>> tempTree = pharmaciesTree;

    // Используем неконстантные итераторы
    for (auto it = tempTree.begin(); it != tempTree.end(); ++it) {
        auto pharmacy = *it;
        int quantity = pharmacy->checkStock(productId);
        if (quantity > 0)
            availability[pharmacy->getId()] = quantity;
    }

    return availability;
}

std::vector<std::pair<std::string, std::string>> PharmacyManager::findProductInPharmacies(const std::string& productNameOrId) const
{
    if (productNameOrId.empty())
        throw InvalidProductDataException("product name or ID", "cannot be empty");

    std::vector<std::pair<std::string, std::string>> result;

    // Обходим все аптеки в дереве
    for (auto it = pharmaciesTree.begin(); it != pharmaciesTree.end(); ++it) {
        auto pharmacy = *it;
        auto product = pharmacy->findProduct(productNameOrId);
        if (product)
            result.emplace_back(pharmacy->getId(), pharmacy->getName());
    }

    return result;
}

std::vector<std::shared_ptr<Medicine>> PharmacyManager::getAnalogues(const std::string& productId) const
{
    if (productId.empty())
        throw InvalidProductDataException("product ID", "cannot be empty");

    std::vector<std::shared_ptr<Medicine>> analogues;
    auto original = std::dynamic_pointer_cast<Medicine>(getProduct(productId));

    if (!original)
        throw InvalidProductDataException("product", "is not a medicine or not found");

    for (const auto& productPair : productsCatalog)
    {
        auto medicine = std::dynamic_pointer_cast<Medicine>(productPair.second);
        if (medicine && medicine->getId() != productId &&
            medicine->getActiveSubstance() == original->getActiveSubstance())
            analogues.push_back(medicine);
    }

    return analogues;
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsByName(const std::string& name) const
{
    if (name.empty())
        throw InvalidProductDataException("name", "cannot be empty");

    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
    {
        auto product = pair.second;
        if (product->getName().find(name) != std::string::npos ||
            product->getId().find(name) != std::string::npos ||
            product->getManufacturerCountry().find(name) != std::string::npos)
            result.push_back(product);
    }
    return result;
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::getAllProducts() const
{
    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
        result.push_back(pair.second);

    return result;
}

void PharmacyManager::displaySupplyInfo() const
{
    std::cout << "=== Supply Operations ===\n";
    for (const auto& operation : operations)
    {
        auto supply = std::dynamic_pointer_cast<Supply>(operation);
        if (supply)
            std::cout << *supply << "---\n";
    }
}

void PharmacyManager::displayReturnInfo() const
{
    std::cout << "=== Return Operations ===\n";
    for (const auto& operation : operations)
    {
        auto returnOp = std::dynamic_pointer_cast<Return>(operation);
        if (returnOp)
            std::cout << *returnOp << "---\n";
    }
}

void PharmacyManager::displayWriteOffInfo() const
{
    std::cout << "=== Write-off Operations ===\n";
    for (const auto& operation : operations)
    {
        auto writeOff = std::dynamic_pointer_cast<WriteOff>(operation);
        if (writeOff)
            std::cout << *writeOff << "---\n";
    }
}

bool PharmacyManager::updateProduct(std::shared_ptr<MedicalProduct> updatedProduct)
{
    std::string id = updatedProduct->getId();

    // Находим продукт по ID в productsCatalog
    auto it = productsCatalog.find(id);
    if (it != productsCatalog.end()) {
        // Заменяем старый продукт на обновленный
        it->second = updatedProduct;
        return true;
    }

    return false;
}

void PharmacyManager::loadSuppliesFromFile(const std::string& filename)
{
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open supplies file");
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                std::istringstream iss(line);
                std::string type;
                std::getline(iss, type, ';');

                if (type == "SUPPLY") {
                    Supply supply;
                    iss >> supply;
                    addOperation(std::make_shared<Supply>(supply));
                }
            }
        }

        file.close();
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading supplies: " + std::string(e.what()));
    }
}

std::vector<std::shared_ptr<InventoryOperation>> PharmacyManager::getAllOperations() const {
    return operations;
}

void PharmacyManager::clearAll() {
    productsCatalog.clear();
    operations.clear();
    pharmaciesTree.clear();  // Очищаем дерево вместо map
}

std::vector<std::shared_ptr<Pharmacy>> PharmacyManager::getAllPharmacies() const {
    std::vector<std::shared_ptr<Pharmacy>> result;

    // Используем итераторы дерева для обхода
    for (auto it = pharmaciesTree.begin(); it != pharmaciesTree.end(); ++it) {
        result.push_back(*it);
    }

    return result;
}

std::shared_ptr<Pharmacy> PharmacyManager::findPharmacyInTree(const std::string& pharmacyId) const
{
    // Теперь find_if возвращает обычный iterator, даже в константном методе
    auto it = pharmaciesTree.find_if([&pharmacyId](const std::shared_ptr<Pharmacy>& pharmacy) {
        return pharmacy->getId() == pharmacyId;
    });

    if (it != pharmaciesTree.end()) {
        return *it;  // Можно разыменовать
    }

    return nullptr;
}
