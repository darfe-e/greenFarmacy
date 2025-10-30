#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "pharmacymanager.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

class FileManager
{
public:
    // Загрузка данных
    static std::map<std::string, std::shared_ptr<MedicalProduct>> loadProducts(const std::string& filename);
    static std::map<std::string, std::shared_ptr<Pharmacy>> loadPharmacies(const std::string& filename);
    static std::vector<std::shared_ptr<InventoryOperation>> loadOperations(const std::string& filename);

    // Сохранение данных
    static void saveProducts(const std::string& filename, const std::map<std::string, std::shared_ptr<MedicalProduct>>& products);
    static void savePharmacies(const std::string& filename, const std::map<std::string, std::shared_ptr<Pharmacy>>& pharmacies);
    static void saveOperations(const std::string& filename, const std::vector<std::shared_ptr<InventoryOperation>>& operations);

private:
    // Вспомогательные методы для парсинга
    static std::shared_ptr<MedicalProduct> parseProductLine(const std::string& line);
    static std::shared_ptr<Pharmacy> parsePharmacyLine(const std::string& line);
    static std::shared_ptr<InventoryOperation> parseOperationLine(const std::string& line);

    // Вспомогательные методы для сериализации
    static std::string serializeProduct(const std::shared_ptr<MedicalProduct>& product);
    static std::string serializePharmacy(const std::shared_ptr<Pharmacy>& pharmacy);
    static std::string serializeOperation(const std::shared_ptr<InventoryOperation>& operation);
};

#endif
