#ifndef PHARMACYMANAGER_H
#define PHARMACYMANAGER_H

#include "pharmacy.h"
#include "medicalproduct.h"
#include "medicine.h"
#include "inventoryoperation.h"
#include "supply.h"
#include "return.h"
#include "writeoff.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/PharmacyExceptions/ProductNotFoundException.h"
#include "Exception/PharmacyExceptions/DuplicateProductException.h"
#include "my_binary_tree/binarytree.h"  // Добавляем ваше бинарное дерево
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <functional>

class PharmacyManager
{
private:
    std::map<std::string, std::shared_ptr<MedicalProduct>> productsCatalog;
    // Заменяем std::map на binaryTree для аптек
    binaryTree<std::shared_ptr<Pharmacy>> pharmaciesTree;
    std::vector<std::shared_ptr<InventoryOperation>> operations;

    // Компаратор для сравнения аптек по ID
    struct PharmacyComparator {
        bool operator()(const std::shared_ptr<Pharmacy>& a, const std::shared_ptr<Pharmacy>& b) const {
            return a->getId() < b->getId();
        }
    };

public:
    PharmacyManager();

    // Управление продуктами
    void addProduct(std::shared_ptr<MedicalProduct> product);
    void removeProduct(const std::string& productId);
    std::shared_ptr<MedicalProduct> getProduct(const std::string& productId) const;

    // Управление аптеками (теперь через бинарное дерево)
    void addPharmacy(std::shared_ptr<Pharmacy> pharmacy);
    void removePharmacy(const std::string& pharmacyId);
    std::shared_ptr<Pharmacy> getPharmacy(const std::string& pharmacyId) const;

    // Управление операциями
    void addOperation(std::shared_ptr<InventoryOperation> operation);
    std::vector<std::shared_ptr<Supply>> getSupplyOperations() const;
    std::vector<std::shared_ptr<Return>> getReturnOperations() const;
    std::vector<std::shared_ptr<WriteOff>> getWriteOffOperations() const;
    std::vector<std::shared_ptr<InventoryOperation>> getAllOperations() const;
    void loadSuppliesFromFile(const std::string& filename);

    // Поиск и аналитика
    std::vector<std::shared_ptr<MedicalProduct>> searchProductsByCountry(const std::string& country) const;
    std::vector<std::shared_ptr<MedicalProduct>> searchProductsBySubstance(const std::string& substance) const;
    std::vector<std::shared_ptr<MedicalProduct>> searchProducts(const std::string& searchTerm) const;
    std::vector<std::shared_ptr<MedicalProduct>> searchProductsByName(const std::string& name) const;
    std::map<std::string, int> getProductAvailability(const std::string& productId) const;
    std::vector<std::pair<std::string, std::string>> findProductInPharmacies(const std::string& productNameOrId) const;
    std::vector<std::shared_ptr<Medicine>> getAnalogues(const std::string& productId) const;
    std::vector<std::shared_ptr<MedicalProduct>> getAllProducts() const;

    bool updateProduct(std::shared_ptr<MedicalProduct> updatedProduct);

    // Отображение информации
    void displaySupplyInfo() const;
    void displayReturnInfo() const;
    void displayWriteOffInfo() const;

    void clearAll();

    // Метод для получения всех аптек из дерева
    std::vector<std::shared_ptr<Pharmacy>> getAllPharmacies() const;

    // Дополнительный метод для демонстрации работы с деревом
    void printPharmaciesTree() const;

private:
    // Вспомогательный метод для поиска аптеки в дереве
    std::shared_ptr<Pharmacy> findPharmacyInTree(const std::string& pharmacyId) const;
};

#endif // PHARMACYMANAGER_H
