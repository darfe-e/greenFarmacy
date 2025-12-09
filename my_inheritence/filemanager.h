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
#include "Files/file_txt.h"
#include "stockrecord.h"

class FileManager
{
private:
    static FileManager* instance;

    // Файлы для разных типов данных
    File_text<std::string> medicinesFile;
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

    // Методы для работы с файлом pharmacies.txt
    bool loadPharmacies(std::vector<std::shared_ptr<Pharmacy>>& pharmacies);  // Изменено
    bool savePharmacies(const std::vector<std::shared_ptr<Pharmacy>>& pharmacies);  // Изменено

    // Методы для работы с файлом stock.txt
    bool loadStockData(std::vector<std::shared_ptr<Pharmacy>>& pharmacies,  // Изменено
                       const std::vector<std::shared_ptr<Medicine>>& medicines);
    bool saveStockData(const std::vector<std::shared_ptr<Pharmacy>>& pharmacies);  // Изменено

    bool loadInventoryOperations(std::vector<std::shared_ptr<InventoryOperation>>& operations);
    bool saveInventoryOperations(const std::vector<std::shared_ptr<InventoryOperation>>& operations);

    bool loadAnalogues(std::vector<std::shared_ptr<Medicine>>& medicines);
    bool saveAnalogues(const std::vector<std::shared_ptr<Medicine>>& medicines);

    std::vector<std::pair<std::string, int>> getAvailabilityInOtherPharmacies(
        const std::string& productId);

};

#endif // FILEMANAGER_H
