#include "pharmacymanager.h"
#include "supply.h"
#include "return.h"
#include "writeoff.h"
#include <algorithm>
#include <iostream>

PharmacyManager::PharmacyManager()
{
}

void PharmacyManager::addProduct(std::shared_ptr<MedicalProduct> product)
{
    if (!product)
        throw std::invalid_argument("Product cannot be null");

    productsCatalog[product->getId()] = product;
}

void PharmacyManager::removeProduct(const std::string& productId)
{
    auto it = productsCatalog.find(productId);
    if (it != productsCatalog.end())
        productsCatalog.erase(it);
}

std::shared_ptr<MedicalProduct> PharmacyManager::getProduct(const std::string& productId) const
{
    auto it = productsCatalog.find(productId);
    if (it != productsCatalog.end())
        return it->second;
    return nullptr;
}

void PharmacyManager::addPharmacy(std::shared_ptr<Pharmacy> pharmacy)
{
    if (!pharmacy)
        throw std::invalid_argument("Pharmacy cannot be null");

    pharmacies[pharmacy->getId()] = pharmacy;
}

void PharmacyManager::removePharmacy(const std::string& pharmacyId)
{
    auto it = pharmacies.find(pharmacyId);
    if (it != pharmacies.end())
        pharmacies.erase(it);
}

std::shared_ptr<Pharmacy> PharmacyManager::getPharmacy(const std::string& pharmacyId) const
{
    auto it = pharmacies.find(pharmacyId);
    if (it != pharmacies.end())
        return it->second;
    return nullptr;
}

void PharmacyManager::addOperation(std::shared_ptr<InventoryOperation> operation)
{
    if (!operation)
        throw std::invalid_argument("Operation cannot be null");

    operations.push_back(operation);
}

std::vector<std::pair<std::string, std::string>> PharmacyManager::findProductInPharmacies(const std::string& productNameOrId) const
{
    std::vector<std::pair<std::string, std::string>> result;

    for (const auto& pharmacyPair : pharmacies)
    {
        auto pharmacy = pharmacyPair.second;
        auto product = pharmacy->findProduct(productNameOrId);
        if (product)
            result.emplace_back(pharmacy->getId(), pharmacy->getName());
    }

    return result;
}

std::vector<std::shared_ptr<Medicine>> PharmacyManager::getAnalogues(const std::string& productId) const
{
    std::vector<std::shared_ptr<Medicine>> analogues;
    auto original = std::dynamic_pointer_cast<Medicine>(getProduct(productId));

    if (!original)
        return analogues;

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
    std::vector<std::shared_ptr<MedicalProduct>> result;

    for (const auto& productPair : productsCatalog)
    {
        if (productPair.second->getName().find(name) != std::string::npos)
            result.push_back(productPair.second);
    }

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

void PharmacyManager::initializeSampleData()
{
    // Добавление тестовых продуктов
    auto medicine1 = std::make_shared<Medicine>("M001", "Paracetamol", "PharmaCo", 15.50, 100,
                                                "2025-12-31", "paracetamol", 500, "mg");
    auto medicine2 = std::make_shared<Medicine>("M002", "Ibuprofen", "HealthInc", 18.75, 50,
                                                "2025-10-15", "ibuprofen", 400, "mg");
    auto medicine3 = std::make_shared<Medicine>("M003", "Aspirin", "MedCorp", 12.30, 200,
                                                "2024-08-20", "acetylsalicylic acid", 100, "mg");

    addProduct(medicine1);
    addProduct(medicine2);
    addProduct(medicine3);

    // Добавление тестовых аптек
    auto pharmacy1 = std::make_shared<Pharmacy>("P001", "Central Pharmacy", "123 Main St", "555-0101");
    auto pharmacy2 = std::make_shared<Pharmacy>("P002", "Northside Pharmacy", "456 North Ave", "555-0102");

    pharmacy1->addProduct(medicine1, 50);
    pharmacy1->addProduct(medicine2, 25);
    pharmacy2->addProduct(medicine1, 30);
    pharmacy2->addProduct(medicine3, 100);

    addPharmacy(pharmacy1);
    addPharmacy(pharmacy2);

    // Добавление тестовых операций
    auto supply1 = std::make_shared<Supply>("OP001", SafeDate("2024-01-15"), medicine1, 50,
                                            "Supplier A", "Central Pharmacy");
    auto return1 = std::make_shared<Return>("OP002", SafeDate("2024-01-20"), medicine2, 5,
                                            "Defective packaging");
    auto writeOff1 = std::make_shared<WriteOff>("OP003", SafeDate("2024-01-25"), medicine3, 10,
                                                "Expired");

    addOperation(supply1);
    addOperation(return1);
    addOperation(writeOff1);
}
