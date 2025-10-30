#ifndef PHARMACYMANAGER_H
#define PHARMACYMANAGER_H

#include "pharmacy.h"
#include "medicine.h"
#include "inventoryoperation.h"
#include <map>
#include <vector>
#include <memory>

class PharmacyManager
{
private:
    std::map<std::string, std::shared_ptr<MedicalProduct>> productsCatalog;
    std::map<std::string, std::shared_ptr<Pharmacy>> pharmacies;
    std::vector<std::shared_ptr<InventoryOperation>> operations;

public:
    PharmacyManager();
    PharmacyManager(const PharmacyManager& other) = delete;
    ~PharmacyManager() = default;

    // Управление продуктами
    void addProduct(std::shared_ptr<MedicalProduct> product);
    void removeProduct(const std::string& productId);
    std::shared_ptr<MedicalProduct> getProduct(const std::string& productId) const;

    // Управление аптеками
    void addPharmacy(std::shared_ptr<Pharmacy> pharmacy);
    void removePharmacy(const std::string& pharmacyId);
    std::shared_ptr<Pharmacy> getPharmacy(const std::string& pharmacyId) const;

    // Управление операциями
    void addOperation(std::shared_ptr<InventoryOperation> operation);

    // Основной функционал
    std::vector<std::pair<std::string, std::string>> findProductInPharmacies(const std::string& productNameOrId) const;
    std::vector<std::shared_ptr<Medicine>> getAnalogues(const std::string& productId) const;

    // Поиск
    std::vector<std::shared_ptr<MedicalProduct>> searchProductsByName(const std::string& name) const;

    // Информация об операциях
    void displaySupplyInfo() const;
    void displayReturnInfo() const;
    void displayWriteOffInfo() const;

    // Инициализация тестовых данных
    void initializeSampleData();

    // Операторы
    PharmacyManager& operator=(const PharmacyManager& other) = delete;
};

#endif
