#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "medicalproduct.h"
#include "medicine.h"
#include "tablet.h"
#include "syrup.h"
#include "ointment.h"
#include "pharmacy.h"
#include "inventoryoperation.h"
#include "supply.h"
#include "return.h"
#include "writeoff.h"
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "safedate.h"
#include "Files/file_txt.h"
#include "stockrecord.h"

class FileManager
{
private:
    static FileManager* instance;

    // Файлы для разных типов данных
    File_text<std::string> medicinesFile; // Меняем на string и парсим вручную
    File_text<Pharmacy> pharmaciesFile;
    File_text<InventoryOperation> inventoryOperationsFile;
    File_text<StockRecord> stockFile;
    File_text<std::string> analoguesFile;

    FileManager();

public:
    // Удаляем копирование и присваивание
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    static FileManager& getInstance();

    // Методы для работы с файлом medicines.txt
    bool loadMedicines(std::vector<std::shared_ptr<Medicine>>& medicines);
    bool saveMedicines(const std::vector<std::shared_ptr<Medicine>>& medicines);
    bool addMedicine(std::shared_ptr<Medicine> medicine);
    bool truncateMedicinesFile();

    // Методы для работы с файлом pharmacies.txt
    bool loadPharmacies(std::vector<Pharmacy>& pharmacies);
    bool savePharmacies(const std::vector<Pharmacy>& pharmacies);

    // Методы для работы с файлом stock.txt
    bool loadStockData(std::vector<Pharmacy>& pharmacies, const std::vector<std::shared_ptr<Medicine>>& medicines);
    bool saveStockData(const std::vector<Pharmacy>& pharmacies);

    // Эффективный поиск препарата по всем аптекам
    std::map<std::string, int> findProductInPharmacies(const std::string& productId);
    std::vector<std::string> findPharmaciesWithProduct(const std::string& productId);

    // Методы для работы с общим файлом инвентарных операций
    bool loadInventoryOperations(std::vector<std::shared_ptr<InventoryOperation>>& operations);
    bool saveInventoryOperations(const std::vector<std::shared_ptr<InventoryOperation>>& operations);
    bool addInventoryOperation(std::shared_ptr<InventoryOperation> operation);


    bool loadAnalogues(std::vector<std::shared_ptr<Medicine>>& medicines);
    bool saveAnalogues(const std::vector<std::shared_ptr<Medicine>>& medicines);
    bool addAnalogue(const std::string& medicineId, const std::string& analogueId);
    bool removeAnalogue(const std::string& medicineId, const std::string& analogueId);
    std::vector<std::string> getMedicineAnalogues(const std::string& medicineId);

    std::vector<std::pair<std::string, int>> getAvailabilityInOtherPharmacies(
        const std::string& productId);

    // Получить список всех аптек (для отображения названий)
    std::map<std::string, std::string> getPharmacyNames() const;

    bool saveAllData(const std::vector<std::shared_ptr<Medicine>>& medicines,
                                  const std::vector<std::shared_ptr<InventoryOperation>>& operations);

    bool loadAllData(std::vector<std::shared_ptr<Medicine>>& medicines,
                                  std::vector<std::shared_ptr<InventoryOperation>>& operations);
};

#endif // FILEMANAGER_H
