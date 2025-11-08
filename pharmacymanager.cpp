#include "pharmacymanager.h"
#include <algorithm>
#include <iostream>
#include "tablet.h"
#include <fstream>
#include <sstream>

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

std::vector<std::shared_ptr<Supply>> PharmacyManager::getSupplyOperations() const
{
    std::vector<std::shared_ptr<Supply>> supplies;
    for (const auto& operation : operations)
    {
        if (auto supply = std::dynamic_pointer_cast<Supply>(operation))
            supplies.push_back(supply);
    }
    return supplies;
}

std::vector<std::shared_ptr<Return>> PharmacyManager::getReturnOperations() const
{
    std::vector<std::shared_ptr<Return>> returns;
    for (const auto& operation : operations)
    {
        if (auto returnOp = std::dynamic_pointer_cast<Return>(operation))
            returns.push_back(returnOp);
    }
    return returns;
}

std::vector<std::shared_ptr<WriteOff>> PharmacyManager::getWriteOffOperations() const
{
    std::vector<std::shared_ptr<WriteOff>> writeOffs;
    for (const auto& operation : operations)
    {
        if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(operation))
            writeOffs.push_back(writeOff);
    }
    return writeOffs;
}


std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsByCountry(const std::string& country) const
{
    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
    {
        if (pair.second->getManufacturerCountry().find(country) != std::string::npos)
            result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProductsBySubstance(const std::string& substance) const
{
    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
    {
        if (auto medicine = std::dynamic_pointer_cast<Medicine>(pair.second))
        {
            if (medicine->getActiveSubstance().find(substance) != std::string::npos)
                result.push_back(medicine);
        }
    }
    return result;
}

std::vector<std::shared_ptr<MedicalProduct>> PharmacyManager::searchProducts(const std::string& searchTerm) const
{
    std::vector<std::shared_ptr<MedicalProduct>> result;
    for (const auto& pair : productsCatalog)
    {
        auto product = pair.second;
        if (product->getName().find(searchTerm) != std::string::npos ||
            product->getId().find(searchTerm) != std::string::npos ||
            product->getManufacturerCountry().find(searchTerm) != std::string::npos)
        {
            result.push_back(product);
        }
        else if (auto medicine = std::dynamic_pointer_cast<Medicine>(product))
        {
            if (medicine->getActiveSubstance().find(searchTerm) != std::string::npos)
                result.push_back(medicine);
        }
    }
    return result;
}

std::map<std::string, int> PharmacyManager::getProductAvailability(const std::string& productId) const
{
    std::map<std::string, int> availability;
    for (const auto& pharmacyPair : pharmacies)
    {
        int quantity = pharmacyPair.second->checkStock(productId);
        if (quantity > 0)
            availability[pharmacyPair.first] = quantity;
    }
    return availability;
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
    for (const auto& pair : productsCatalog)
    {
        auto product = pair.second;
        // Поиск по названию, ID или стране
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

