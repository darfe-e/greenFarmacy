#include "filemanager.h"
#include <sstream>
#include <stdexcept>
#include <memory>
#include <map>

FileManager* FileManager::instance = nullptr;

FileManager::FileManager()
    : medicinesFile("medicines.txt")
    , pharmaciesFile("pharmacies.txt")
    , inventoryOperationsFile("inventory_operations.txt")
    , stockFile("stock.txt")
{
}

FileManager& FileManager::getInstance() {
    if (instance == nullptr) {
        instance = new FileManager();
    }
    return *instance;
}

// Методы для работы с файлом medicines.txt
bool FileManager::loadMedicines(std::vector<std::shared_ptr<Medicine>>& medicines) {
    try {
        if (!medicinesFile.Open_file_in()) return false;

        medicines.clear();
        std::string line;

        while (!medicinesFile.R_end_file()) {
            medicinesFile.Read_string_line(line);
            if (!medicinesFile.R_end_file() && !line.empty()) {

                // Определяем тип по маркеру в начале строки
                if (line.find("[TABLET];") == 0) {
                    Tablet tablet;
                    std::istringstream iss(line.substr(9)); // Пропускаем маркер
                    iss >> tablet;
                    medicines.push_back(std::make_shared<Tablet>(tablet));
                }
                else if (line.find("[SYRUP];") == 0) {
                    Syrup syrup;
                    std::istringstream iss(line.substr(8)); // Пропускаем маркер
                    iss >> syrup;
                    medicines.push_back(std::make_shared<Syrup>(syrup));
                }
                else if (line.find("[OINTMENT];") == 0) {
                    Ointment ointment;
                    std::istringstream iss(line.substr(11)); // Пропускаем маркер
                    iss >> ointment;
                    medicines.push_back(std::make_shared<Ointment>(ointment));
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileManager::saveMedicines(const std::vector<std::shared_ptr<Medicine>>& medicines) {
    try {
        if (!medicinesFile.Open_file_out()) return false;

        for (const auto& med : medicines) {
            if (med) {
                // Используем dynamic_cast для определения типа и записываем как строку
                if (auto tablet = std::dynamic_pointer_cast<Tablet>(med)) {
                    Tablet tempTablet = *tablet;
                    std::ostringstream oss;
                    oss << tempTablet;
                    medicinesFile.Write_string_line(oss.str());
                }
                else if (auto syrup = std::dynamic_pointer_cast<Syrup>(med)) {
                    Syrup tempSyrup = *syrup;
                    std::ostringstream oss;
                    oss << tempSyrup;
                    medicinesFile.Write_string_line(oss.str());
                }
                else if (auto ointment = std::dynamic_pointer_cast<Ointment>(med)) {
                    Ointment tempOintment = *ointment;
                    std::ostringstream oss;
                    oss << tempOintment;
                    medicinesFile.Write_string_line(oss.str());
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileManager::addMedicine(std::shared_ptr<Medicine> medicine) {
    try {
        if (!medicinesFile.Open_file_out()) return false;
        if (medicine) {
            if (auto tablet = std::dynamic_pointer_cast<Tablet>(medicine)) {
                Tablet tempTablet = *tablet;
                std::ostringstream oss;
                oss << tempTablet;
                medicinesFile.Write_string_line(oss.str());
            }
            else if (auto syrup = std::dynamic_pointer_cast<Syrup>(medicine)) {
                Syrup tempSyrup = *syrup;
                std::ostringstream oss;
                oss << tempSyrup;
                medicinesFile.Write_string_line(oss.str());
            }
            else if (auto ointment = std::dynamic_pointer_cast<Ointment>(medicine)) {
                Ointment tempOintment = *ointment;
                std::ostringstream oss;
                oss << tempOintment;
                medicinesFile.Write_string_line(oss.str());
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Методы для работы с файлом pharmacies.txt
bool FileManager::loadPharmacies(std::vector<Pharmacy>& pharmacies) {
    try {
        if (!pharmaciesFile.Open_file_in()) return false;

        pharmacies.clear();
        Pharmacy pharmacy;
        while (!pharmaciesFile.R_end_file()) {
            pharmaciesFile.Read_record_in_file_text(pharmacy);
            if (!pharmaciesFile.R_end_file()) {
                pharmacies.push_back(pharmacy);
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileManager::savePharmacies(const std::vector<Pharmacy>& pharmacies) {
    try {
        if (!pharmaciesFile.Open_file_out()) return false;

        for (const auto& pharmacy : pharmacies) {
            Pharmacy nonConstPharmacy = pharmacy;
            pharmaciesFile.Write_record_in_file_text(nonConstPharmacy);
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Методы для работы с файлом stock.txt (остатки товаров)
bool FileManager::loadStockData(std::vector<Pharmacy>& pharmacies, const std::vector<std::shared_ptr<Medicine>>& medicines) {
    try {
        if (!stockFile.Open_file_in()) return false;

        // Создаем карту лекарств для быстрого поиска по ID
        std::map<std::string, std::shared_ptr<Medicine>> medicineMap;
        for (const auto& med : medicines) {
            if (med) {
                medicineMap[med->getId()] = med;
            }
        }

        // Создаем карту аптек для быстрого поиска по ID
        std::map<std::string, Pharmacy*> pharmacyMap;
        for (auto& pharmacy : pharmacies) {
            pharmacyMap[pharmacy.getId()] = &pharmacy;
        }

        StockRecord record;
        while (!stockFile.R_end_file()) {
            stockFile.Read_record_in_file_text(record);
            if (!stockFile.R_end_file()) {
                // Находим аптеку и добавляем товар в ее склад
                auto pharmacyIt = pharmacyMap.find(record.pharmacyId);
                auto medicineIt = medicineMap.find(record.productId);

                if (pharmacyIt != pharmacyMap.end() && medicineIt != medicineMap.end()) {
                    pharmacyIt->second->addToStorage(medicineIt->second, record.quantity);
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileManager::saveStockData(const std::vector<Pharmacy>& pharmacies) {
    try {
        if (!stockFile.Open_file_out()) return false;

        for (const auto& pharmacy : pharmacies) {
            auto products = pharmacy.getAllProducts();
            for (const auto& productPair : products) {
                const auto& product = productPair.first;
                int quantity = productPair.second;

                if (product && quantity > 0) {
                    // Получаем текущую дату
                    std::time_t t = std::time(nullptr);
                    std::tm* now = std::localtime(&t);
                    SafeDate currentDate(*now);

                    StockRecord record(product->getId(), pharmacy.getId(), quantity, currentDate);
                    stockFile.Write_record_in_file_text(record);
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::map<std::string, int> FileManager::findProductInPharmacies(const std::string& productId) {
    std::map<std::string, int> result;

    try {
        if (!stockFile.Open_file_in()) return result;

        StockRecord record;
        while (!stockFile.R_end_file()) {
            stockFile.Read_record_in_file_text(record);
            if (!stockFile.R_end_file() && record.productId == productId && record.quantity > 0) {
                result[record.pharmacyId] = record.quantity;
            }
        }
    } catch (const std::exception& e) {
        // Игнорируем ошибки при поиске
    }

    return result;
}

std::vector<std::string> FileManager::findPharmaciesWithProduct(const std::string& productId) {
    std::vector<std::string> result;

    auto pharmaciesWithProduct = findProductInPharmacies(productId);
    for (const auto& pair : pharmaciesWithProduct) {
        result.push_back(pair.first);
    }

    return result;
}

// Методы для работы с общим файлом инвентарных операций
bool FileManager::loadInventoryOperations(std::vector<std::shared_ptr<InventoryOperation>>& operations) {
    try {
        if (!inventoryOperationsFile.Open_file_in()) return false;

        operations.clear();

        // Временные объекты для чтения
        Supply supply;
        Return returnOp;
        WriteOff writeOff;

        while (!inventoryOperationsFile.R_end_file()) {
            // Читаем строку и анализируем ее для определения типа операции
            std::string line;
            inventoryOperationsFile.Read_string_line(line);

            if (!inventoryOperationsFile.R_end_file() && !line.empty()) {
                std::istringstream iss(line);

                // Пытаемся определить тип операции по первому полю или структуре данных
                // Сначала пробуем прочитать как Supply
                iss >> supply;
                if (!iss.fail()) {
                    operations.push_back(std::make_shared<Supply>(supply));
                    continue;
                }

                // Если не Supply, пробуем Return
                iss.clear();
                iss.str(line);
                iss >> returnOp;
                if (!iss.fail()) {
                    operations.push_back(std::make_shared<Return>(returnOp));
                    continue;
                }

                // Если не Return, пробуем WriteOff
                iss.clear();
                iss.str(line);
                iss >> writeOff;
                if (!iss.fail()) {
                    operations.push_back(std::make_shared<WriteOff>(writeOff));
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileManager::saveInventoryOperations(const std::vector<std::shared_ptr<InventoryOperation>>& operations) {
    try {
        if (!inventoryOperationsFile.Open_file_out()) return false;

        for (const auto& operation : operations) {
            if (operation) {
                // Используем dynamic_cast для определения конкретного типа
                if (auto supply = std::dynamic_pointer_cast<Supply>(operation)) {
                    Supply nonConstSupply = *supply;
                    inventoryOperationsFile.Write_record_in_file_text(nonConstSupply);
                } else if (auto returnOp = std::dynamic_pointer_cast<Return>(operation)) {
                    Return nonConstReturn = *returnOp;
                    inventoryOperationsFile.Write_record_in_file_text(nonConstReturn);
                } else if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(operation)) {
                    WriteOff nonConstWriteOff = *writeOff;
                    inventoryOperationsFile.Write_record_in_file_text(nonConstWriteOff);
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileManager::addInventoryOperation(std::shared_ptr<InventoryOperation> operation) {
    try {
        if (!inventoryOperationsFile.Open_file_out()) return false;

        if (operation) {
            if (auto supply = std::dynamic_pointer_cast<Supply>(operation)) {
                Supply nonConstSupply = *supply;
                inventoryOperationsFile.Write_record_in_file_text(nonConstSupply);
            } else if (auto returnOp = std::dynamic_pointer_cast<Return>(operation)) {
                Return nonConstReturn = *returnOp;
                inventoryOperationsFile.Write_record_in_file_text(nonConstReturn);
            } else if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(operation)) {
                WriteOff nonConstWriteOff = *writeOff;
                inventoryOperationsFile.Write_record_in_file_text(nonConstWriteOff);
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Закрытие всех файлов
void FileManager::closeAllFiles() {
    // Файлы автоматически закрываются в деструкторах File_text
}
