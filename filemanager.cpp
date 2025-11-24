#include "filemanager.h"
#include <sstream>
#include <stdexcept>
#include <memory>
#include <map>
#include "QDebug"

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

bool FileManager::truncateMedicinesFile() {
    try {
        if (!medicinesFile.Open_file_trunc()) {
            qDebug() << "Не удалось открыть medicines.txt для очистки";
            return false;
        }
        qDebug() << "Файл medicines.txt очищен";
        return true;
    } catch (const std::exception& e) {
        qDebug() << "Ошибка при очистке файла medicines.txt:" << e.what();
        return false;
    }
}

// Методы для работы с файлом medicines.txt
bool FileManager::loadMedicines(std::vector<std::shared_ptr<Medicine>>& medicines) {
    try {
        qDebug() << "=== ЗАГРУЗКА ЛЕКАРСТВ ИЗ ФАЙЛА ===";

        // ЗАКРЫВАЕМ ФАЙЛ ПЕРЕД ОТКРЫТИЕМ ДЛЯ ЧТЕНИЯ
        medicinesFile.Close_file_in();

        if (!medicinesFile.Open_file_in()) {
            qDebug() << "Файл medicines.txt не существует или не может быть открыт";
            return false;
        }

        medicines.clear();
        std::string line;
        int lineNumber = 0;
        int loadedCount = 0;

        // ИСПРАВЛЕННАЯ ЛОГИКА ЧТЕНИЯ
        while (true) {
            try {
                medicinesFile.Read_string_line(line);
                lineNumber++;
                qDebug() << "Строка" << lineNumber << ":" << QString::fromStdString(line);

                // Обработка строки
                if (line.find("[TABLET];") == 0) {
                    Tablet tablet;
                    std::string remainingLine = line.substr(9);
                    std::istringstream tabletStream(remainingLine);
                    tabletStream >> tablet;
                    if (!tabletStream.fail()) {
                        medicines.push_back(std::make_shared<Tablet>(tablet));
                        loadedCount++;
                        qDebug() << "✓ Загружена таблетка:" << QString::fromStdString(tablet.getName());
                    } else {
                        qDebug() << "✗ Ошибка парсинга таблетки";
                    }
                }
                else if (line.find("[SYRUP];") == 0) {
                    Syrup syrup;
                    std::string remainingLine = line.substr(8);
                    std::istringstream syrupStream(remainingLine);
                    syrupStream >> syrup;
                    if (!syrupStream.fail()) {
                        medicines.push_back(std::make_shared<Syrup>(syrup));
                        loadedCount++;
                        qDebug() << "✓ Загружен сироп:" << QString::fromStdString(syrup.getName());
                    } else {
                        qDebug() << "✗ Ошибка парсинга сиропа";
                    }
                }
                else if (line.find("[OINTMENT];") == 0) {
                    Ointment ointment;
                    std::string remainingLine = line.substr(11);
                    std::istringstream ointmentStream(remainingLine);
                    ointmentStream >> ointment;
                    if (!ointmentStream.fail()) {
                        medicines.push_back(std::make_shared<Ointment>(ointment));
                        loadedCount++;
                        qDebug() << "✓ Загружена мазь:" << QString::fromStdString(ointment.getName());
                    } else {
                        qDebug() << "✗ Ошибка парсинга мази";
                    }
                } else {
                    qDebug() << "✗ Неизвестный тип в строке:" << lineNumber;
                }
            }
            catch (const std::runtime_error& e) {
                // Ловим исключение конца файла и выходим из цикла
                if (std::string(e.what()).find("End of file") != std::string::npos) {
                    qDebug() << "Достигнут конец файла";
                    break;
                } else {
                    // Другие ошибки - логируем и продолжаем
                    qDebug() << "Ошибка чтения строки" << lineNumber << ":" << e.what();
                }
            }
        }

        medicinesFile.Close_file_in();
        qDebug() << "=== ЗАГРУЗКА ЗАВЕРШЕНА: " << loadedCount << "лекарств ===";
        return true;
    } catch (const std::exception& e) {
        qDebug() << "!!! ОБЩАЯ ОШИБКА ЗАГРУЗКИ:" << e.what();
        medicinesFile.Close_file_in();
        return false;
    }
}

bool FileManager::saveMedicines(const std::vector<std::shared_ptr<Medicine>>& medicines) {
    try {
        qDebug() << "=== СОХРАНЕНИЕ" << medicines.size() << "ЛЕКАРСТВ В ФАЙЛ ===";

        // Закрываем файл перед открытием в новом режиме
        medicinesFile.Close_file_o();

        if (!medicinesFile.Open_file_trunc()) {  // Используем trunc для полной перезаписи
            qDebug() << "Не удалось открыть medicines.txt для записи";
            return false;
        }

        int savedCount = 0;
        for (const auto& med : medicines) {
            if (med) {
                try {
                    if (auto tablet = std::dynamic_pointer_cast<Tablet>(med)) {
                        Tablet tempTablet = *tablet;
                        std::ostringstream oss;
                        oss << tempTablet;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                        savedCount++;
                        qDebug() << "✓ Сохранена таблетка:" << QString::fromStdString(tablet->getName());
                    }
                    else if (auto syrup = std::dynamic_pointer_cast<Syrup>(med)) {
                        Syrup tempSyrup = *syrup;
                        std::ostringstream oss;
                        oss << tempSyrup;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                        savedCount++;
                        qDebug() << "✓ Сохранен сироп:" << QString::fromStdString(syrup->getName());
                    }
                    else if (auto ointment = std::dynamic_pointer_cast<Ointment>(med)) {
                        Ointment tempOintment = *ointment;
                        std::ostringstream oss;
                        oss << tempOintment;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                        savedCount++;
                        qDebug() << "✓ Сохранена мазь:" << QString::fromStdString(ointment->getName());
                    }
                } catch (const std::exception& e) {
                    qDebug() << "✗ Ошибка сохранения лекарства:" << e.what();
                }
            }
        }

        medicinesFile.Flush();  // Сбрасываем буфер
        medicinesFile.Close_file_o();  // Закрываем файл

        qDebug() << "=== СОХРАНЕНИЕ ЗАВЕРШЕНО: " << savedCount << "лекарств ===";
        return true;
    } catch (const std::exception& e) {
        qDebug() << "!!! ОБЩАЯ ОШИБКА СОХРАНЕНИЯ:" << e.what();
        return false;
    }
}

bool FileManager::addMedicine(std::shared_ptr<Medicine> medicine) {
    try {
        qDebug() << "=== ДОБАВЛЕНИЕ ЛЕКАРСТВА В ФАЙЛ ===";

        // ОТКРЫВАЕМ ФАЙЛ В РЕЖИМЕ APPEND (добавление в конец)
        medicinesFile.Close_file_o();
        if (!medicinesFile.Open_file_out()) {
            qDebug() << "Не удалось открыть medicines.txt для добавления";
            return false;
        }

        if (medicine) {
            try {
                if (auto tablet = std::dynamic_pointer_cast<Tablet>(medicine)) {
                    // ПРАВИЛЬНЫЙ СПОСОБ: создаем строку и записываем ее
                    Tablet tempTablet = *tablet;
                    std::ostringstream oss;
                    oss << "[TABLET];" << tempTablet;
                    std::string line = oss.str();
                    medicinesFile.Write_string_line(line);
                    qDebug() << "✓ Добавлена таблетка:" << QString::fromStdString(tablet->getName());
                    qDebug() << "  Данные:" << QString::fromStdString(line);
                }
                else if (auto syrup = std::dynamic_pointer_cast<Syrup>(medicine)) { // ИСПРАВЛЕНО: Syrup
                    // ПРАВИЛЬНЫЙ СПОСОБ: создаем строку и записываем ее
                    Syrup tempSyrup = *syrup;
                    std::ostringstream oss;
                    oss << "[SYRUP];" << tempSyrup;
                    std::string line = oss.str();
                    medicinesFile.Write_string_line(line);
                    qDebug() << "✓ Добавлен сироп:" << QString::fromStdString(syrup->getName());
                    qDebug() << "  Данные:" << QString::fromStdString(line);
                }
                else if (auto ointment = std::dynamic_pointer_cast<Ointment>(medicine)) {
                    // ПРАВИЛЬНЫЙ СПОСОБ: создаем строку и записываем ее
                    Ointment tempOintment = *ointment;
                    std::ostringstream oss;
                    oss << "[OINTMENT];" << tempOintment;
                    std::string line = oss.str();
                    medicinesFile.Write_string_line(line);
                    qDebug() << "✓ Добавлена мазь:" << QString::fromStdString(ointment->getName());
                    qDebug() << "  Данные:" << QString::fromStdString(line);
                }
            } catch (const std::exception& e) {
                qDebug() << "✗ Ошибка добавления лекарства:" << e.what();
                medicinesFile.Close_file_o();
                return false;
            }
        }

        medicinesFile.Flush();
        medicinesFile.Close_file_o();
        qDebug() << "=== ЛЕКАРСТВО УСПЕШНО ДОБАВЛЕНО ===";
        return true;
    } catch (const std::exception& e) {
        qDebug() << "!!! ОБЩАЯ ОШИБКА ДОБАВЛЕНИЯ:" << e.what();
        medicinesFile.Close_file_o();
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
