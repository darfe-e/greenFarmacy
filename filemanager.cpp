#include "filemanager.h"
#include <sstream>
#include <stdexcept>
#include <memory>
#include <map>
#include <QDebug>
#include "Exception/FileExceptions/FileWriteException.h"
#include "Exception/FileExceptions/FileNotFoundException.h"
#include "Exception/FileExceptions/FileParseException.h"
#include "Exception/FileExceptions/SerializationException.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/safeinput.h"


FileManager* FileManager::instance = nullptr;

FileManager::FileManager()
    : medicinesFile("medicines.txt")
    , pharmaciesFile("pharmacies.txt")
    , inventoryOperationsFile("inventory_operations.txt")
    , stockFile("stock.txt")
    , analoguesFile("analogues.txt")  // ДОБАВЛЯЕМ
{
}

FileManager& FileManager::getInstance()
{
    if (instance == nullptr) 
        instance = new FileManager();
    return *instance;
}

bool FileManager::truncateMedicinesFile()
{
    try
    {
        if (!medicinesFile.Open_file_trunc())
            throw FileWriteException("Failed to truncate medicines file");
        return true;
    }
    catch (const FileWriteException& e)
    {
        return false;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::loadMedicines(std::vector<std::shared_ptr<Medicine>>& medicines)
{
    try
    {
        medicinesFile.Close_file_in();

        if (!medicinesFile.Open_file_in())
            throw FileNotFoundException("medicines.txt");

        medicines.clear();
        std::string line;

        while (true)
        {
            try
            {
                medicinesFile.Read_string_line(line);


                // Обработка строки
                if (line.find("[TABLET];") == 0)
                {
                    Tablet tablet;
                    std::string remainingLine = line.substr(9);
                    std::istringstream tabletStream(remainingLine);
                    tabletStream >> tablet;
                    if (!tabletStream.fail())
                        medicines.push_back(std::make_shared<Tablet>(tablet));
                    else
                        throw FileParseException("Failed to parse tablet data");
                }
                else if (line.find("[SYRUP];") == 0)
                {
                    Syrup syrup;
                    std::string remainingLine = line.substr(8);
                    std::istringstream syrupStream(remainingLine);
                    syrupStream >> syrup;
                    if (!syrupStream.fail())
                        medicines.push_back(std::make_shared<Syrup>(syrup));
                    else
                        throw FileParseException("Failed to parse syrup data");
                }
                else if (line.find("[OINTMENT];") == 0)
                {
                    Ointment ointment;
                    std::string remainingLine = line.substr(11);
                    std::istringstream ointmentStream(remainingLine);
                    ointmentStream >> ointment;
                    if (!ointmentStream.fail())
                    {
                        medicines.push_back(std::make_shared<Ointment>(ointment));
                    }
                    else
                        throw FileParseException("Failed to parse ointment data");
                }
                else
                    throw FileParseException("Unknown medicine type marker");

            }
            catch (const FileParseException& e)
            {
                // Ловим исключение конца файла и выходим из цикла
                if (std::string(e.what()).find("End of file") != std::string::npos)
                    break;
                else
                    continue;
            }
            catch (const SerializationException& e)
            {
                continue;
            }
        }

        medicinesFile.Close_file_in();
        if (!loadAnalogues(medicines))
            qDebug() << "Предупреждение: не удалось загрузить аналоги";
        return true;
    }
    catch (const FileNotFoundException& e)
    {
        medicinesFile.Close_file_in();
        return false;
    }
    catch (const FileParseException& e)
    {
        medicinesFile.Close_file_in();
        return false;
    }
    catch (const std::exception& e)
    {
        medicinesFile.Close_file_in();
        return false;
    }
}

bool FileManager::saveMedicines(const std::vector<std::shared_ptr<Medicine>>& medicines)
{
    try {
        medicinesFile.Close_file_o();

        if (!medicinesFile.Open_file_trunc())
            throw FileWriteException("Failed to open medicines.txt for writing");

        for (const auto& med : medicines)
        {
            if (med) {
                try {
                    if (auto tablet = std::dynamic_pointer_cast<Tablet>(med))
                    {
                        Tablet tempTablet = *tablet;
                        std::ostringstream oss;
                        oss << tempTablet;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                    }
                    else if (auto syrup = std::dynamic_pointer_cast<Syrup>(med))
                    {
                        Syrup tempSyrup = *syrup;
                        std::ostringstream oss;
                        oss << tempSyrup;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                    }
                    else if (auto ointment = std::dynamic_pointer_cast<Ointment>(med))
                    {
                        Ointment tempOintment = *ointment;
                        std::ostringstream oss;
                        oss << tempOintment;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                    }
                }
                catch (const FileWriteException& e)
                {
                    continue;
                }
                catch (const std::exception& e)
                {
                    continue;
                }
            }
        }

        medicinesFile.Flush();
        medicinesFile.Close_file_o();
        return true;
    }
    catch (const FileWriteException& e)
    {
        medicinesFile.Close_file_o();
        return false;
    }
    catch (const std::exception& e)
    {
        medicinesFile.Close_file_o();
        return false;
    }
}

bool FileManager::addMedicine(std::shared_ptr<Medicine> medicine)
{
    try {
        if (!medicine) return false;

        // Базовая валидация с использованием SafeInput
        SafeInput::validateProductId(medicine->getId());
        SafeInput::validateTextField(medicine->getName(), "Name");
        SafeInput::validateTextField(medicine->getManufacturerCountry(), "Country");
        SafeInput::validateTextField(medicine->getActiveSubstance(), "Active substance");

        if (medicine->getBasePrice() <= 0) {
            return false;
        }

        medicinesFile.Close_file_o();
        if (!medicinesFile.Open_file_out()) return false;

        // Запись в файл
        if (auto tablet = std::dynamic_pointer_cast<Tablet>(medicine)) {
            std::ostringstream oss;
            oss << *tablet;
            medicinesFile.Write_string_line(oss.str());
        }
        else if (auto syrup = std::dynamic_pointer_cast<Syrup>(medicine)) {
            std::ostringstream oss;
            oss << *syrup;
            medicinesFile.Write_string_line(oss.str());
        }
        else if (auto ointment = std::dynamic_pointer_cast<Ointment>(medicine)) {
            std::ostringstream oss;
            oss << *ointment;
            medicinesFile.Write_string_line(oss.str());
        }
        else {
            return false;
        }

        medicinesFile.Flush();
        medicinesFile.Close_file_o();
        return true;

    } catch (...) {
        medicinesFile.Close_file_o();
        return false;
    }
}

bool FileManager::loadPharmacies(std::vector<Pharmacy>& pharmacies)
{
    try
    {
        pharmaciesFile.Close_file_in();

        if (!pharmaciesFile.Open_file_in())
            throw FileNotFoundException("pharmacies.txt");

        pharmacies.clear();
        Pharmacy pharmacy;

        while (!pharmaciesFile.R_end_file())
        {
            try
            {
                pharmaciesFile.Read_record_in_file_text(pharmacy);
                if (!pharmaciesFile.R_end_file())
                    pharmacies.push_back(pharmacy);
            }
            catch (const SerializationException& e)
            {
                continue;
            }
            catch (const FileParseException& e)
            {
                if (std::string(e.what()).find("End of file") != std::string::npos)
                    break;
                continue;
            }
        }

        pharmaciesFile.Close_file_in();
        return true;
    }
    catch (const FileNotFoundException& e)
    {
        pharmaciesFile.Close_file_in();
        return false;
    }
    catch (const std::exception& e)
    {
        pharmaciesFile.Close_file_in();
        return false;
    }
}

bool FileManager::savePharmacies(const std::vector<Pharmacy>& pharmacies)
{
    try
    {
        if (!pharmaciesFile.Open_file_out()) return false;

        for (const auto& pharmacy : pharmacies)
        {
            Pharmacy nonConstPharmacy = pharmacy;
            pharmaciesFile.Write_record_in_file_text(nonConstPharmacy);
        }
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

// Методы для работы с файлом stock.txt (остатки товаров)
bool FileManager::loadStockData(std::vector<Pharmacy>& pharmacies, const std::vector<std::shared_ptr<Medicine>>& medicines)
{
    try
    {
        if (!stockFile.Open_file_in()) return false;

        // Создаем карту лекарств для быстрого поиска по ID
        std::map<std::string, std::shared_ptr<Medicine>> medicineMap;
        for (const auto& med : medicines)
        {
            if (med) 
                medicineMap[med->getId()] = med;
        }

        // Создаем карту аптек для быстрого поиска по ID
        std::map<std::string, Pharmacy*> pharmacyMap;
        for (auto& pharmacy : pharmacies) 
            pharmacyMap[pharmacy.getId()] = &pharmacy;

        StockRecord record;
        while (!stockFile.R_end_file())
        {
            stockFile.Read_record_in_file_text(record);
            if (!stockFile.R_end_file())
            {
                // Находим аптеку и добавляем товар в ее склад
                auto pharmacyIt = pharmacyMap.find(record.pharmacyId);
                auto medicineIt = medicineMap.find(record.productId);

                if (pharmacyIt != pharmacyMap.end() && medicineIt != medicineMap.end()) 
                    pharmacyIt->second->addToStorage(medicineIt->second, record.quantity);
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::saveStockData(const std::vector<Pharmacy>& pharmacies)
{
    try
    {
        if (!stockFile.Open_file_out()) return false;

        for (const auto& pharmacy : pharmacies)
        {
            auto products = pharmacy.getAllProducts();
            for (const auto& productPair : products)
            {
                const auto& product = productPair.first;
                int quantity = productPair.second;

                if (product && quantity > 0)
                {
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
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

std::map<std::string, int> FileManager::findProductInPharmacies(const std::string& productId)
{
    std::map<std::string, int> result;

    try
    {
        if (!stockFile.Open_file_in()) return result;

        StockRecord record;
        while (!stockFile.R_end_file())
        {
            stockFile.Read_record_in_file_text(record);
            if (!stockFile.R_end_file() && record.productId == productId && record.quantity > 0) 
                result[record.pharmacyId] = record.quantity;
        }
    }
    catch (const std::exception& e)
    {
        // Игнорируем ошибки при поиске
    }

    return result;
}

std::vector<std::string> FileManager::findPharmaciesWithProduct(const std::string& productId)
{
    std::vector<std::string> result;

    auto pharmaciesWithProduct = findProductInPharmacies(productId);
    for (const auto& pair : pharmaciesWithProduct) 
        result.push_back(pair.first);

    return result;
}

// Методы для работы с общим файлом инвентарных операций
bool FileManager::loadInventoryOperations(std::vector<std::shared_ptr<InventoryOperation>>& operations)
{
    try
    {
        if (!inventoryOperationsFile.Open_file_in()) return false;

        operations.clear();

        // Временные объекты для чтения
        Supply supply;
        Return returnOp;
        WriteOff writeOff;

        while (!inventoryOperationsFile.R_end_file())
        {
            // Читаем строку и анализируем ее для определения типа операции
            std::string line;
            inventoryOperationsFile.Read_string_line(line);

            if (!inventoryOperationsFile.R_end_file() && !line.empty())
            {
                std::istringstream iss(line);

                // Пытаемся определить тип операции по первому полю или структуре данных
                // Сначала пробуем прочитать как Supply
                iss >> supply;
                if (!iss.fail())
                {
                    operations.push_back(std::make_shared<Supply>(supply));
                    continue;
                }

                // Если не Supply, пробуем Return
                iss.clear();
                iss.str(line);
                iss >> returnOp;
                if (!iss.fail())
                {
                    operations.push_back(std::make_shared<Return>(returnOp));
                    continue;
                }

                // Если не Return, пробуем WriteOff
                iss.clear();
                iss.str(line);
                iss >> writeOff;
                if (!iss.fail()) 
                    operations.push_back(std::make_shared<WriteOff>(writeOff));
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::saveInventoryOperations(const std::vector<std::shared_ptr<InventoryOperation>>& operations)
{
    try
    {
        if (!inventoryOperationsFile.Open_file_out()) return false;

        for (const auto& operation : operations)
        {
            if (operation)
            {
                // Используем dynamic_cast для определения конкретного типа
                if (auto supply = std::dynamic_pointer_cast<Supply>(operation))
                {
                    Supply nonConstSupply = *supply;
                    inventoryOperationsFile.Write_record_in_file_text(nonConstSupply);
                }
                else if (auto returnOp = std::dynamic_pointer_cast<Return>(operation))
                {
                    Return nonConstReturn = *returnOp;
                    inventoryOperationsFile.Write_record_in_file_text(nonConstReturn);
                }
                else if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(operation))
                {
                    WriteOff nonConstWriteOff = *writeOff;
                    inventoryOperationsFile.Write_record_in_file_text(nonConstWriteOff);
                }
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::addInventoryOperation(std::shared_ptr<InventoryOperation> operation)
{
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

// Загрузка аналогов из файла
bool FileManager::loadAnalogues(std::vector<std::shared_ptr<Medicine>>& medicines)
{
    try {
        analoguesFile.Close_file_in();

        if (!analoguesFile.Open_file_in()) {
            // Файл может не существовать - это нормально
            return true;
        }

        // Создаем карту лекарств для быстрого поиска по ID
        std::map<std::string, std::shared_ptr<Medicine>> medicineMap;
        for (const auto& med : medicines) {
            if (med) {
                medicineMap[med->getId()] = med;
            }
        }

        std::string line;
        while (true) {
            try {
                analoguesFile.Read_string_line(line);

                // Формат: medicineId;analogueId
                size_t delimiterPos = line.find(';');
                if (delimiterPos != std::string::npos) {
                    std::string medicineId = line.substr(0, delimiterPos);
                    std::string analogueId = line.substr(delimiterPos + 1);

                    // Находим лекарство и аналог
                    auto medicineIt = medicineMap.find(medicineId);
                    auto analogueIt = medicineMap.find(analogueId);

                    if (medicineIt != medicineMap.end() && analogueIt != medicineMap.end()) {
                        // Добавляем аналог
                        medicineIt->second->addAnalogue(analogueIt->second);
                    }
                }
            }
            catch (const FileParseException& e) {
                if (std::string(e.what()).find("End of file") != std::string::npos) {
                    break;
                }
                // Игнорируем другие ошибки парсинга и продолжаем
                continue;
            }
        }

        analoguesFile.Close_file_in();
        return true;
    }
    catch (const std::exception& e) {
        analoguesFile.Close_file_in();
        return false;
    }
}

// Сохранение всех аналогов
bool FileManager::saveAnalogues(const std::vector<std::shared_ptr<Medicine>>& medicines)
{
    try {
        analoguesFile.Close_file_o();

        if (!analoguesFile.Open_file_trunc()) {
            return false;
        }

        for (const auto& medicine : medicines) {
            if (medicine) {
                auto analogueIds = medicine->getAnalogueIds();
                for (const auto& analogueId : analogueIds) {
                    // Записываем в формате: medicineId;analogueId
                    std::string line = medicine->getId() + ";" + analogueId;
                    analoguesFile.Write_string_line(line);
                }
            }
        }

        analoguesFile.Flush();
        analoguesFile.Close_file_o();
        return true;
    }
    catch (const std::exception& e) {
        analoguesFile.Close_file_o();
        return false;
    }
}

// Добавление одного аналога
bool FileManager::addAnalogue(const std::string& medicineId, const std::string& analogueId)
{
    try {
        analoguesFile.Close_file_o();

        if (!analoguesFile.Open_file_out()) {
            return false;
        }

        // Записываем в формате: medicineId;analogueId
        std::string line = medicineId + ";" + analogueId;
        analoguesFile.Write_string_line(line);

        analoguesFile.Flush();
        analoguesFile.Close_file_o();
        return true;
    }
    catch (const std::exception& e) {
        analoguesFile.Close_file_o();
        return false;
    }
}

// Удаление аналога
bool FileManager::removeAnalogue(const std::string& medicineId, const std::string& analogueId)
{
    try {
        // Для удаления нужно перезаписать весь файл без удаляемой записи
        analoguesFile.Close_file_in();
        analoguesFile.Close_file_o();

        if (!analoguesFile.Open_file_in()) {
            return true; // Файл не существует - значит нечего удалять
        }

        // Читаем все строки кроме удаляемой
        std::vector<std::string> lines;
        std::string line;
        std::string targetLine = medicineId + ";" + analogueId;

        while (true) {
            try {
                analoguesFile.Read_string_line(line);
                if (line != targetLine) {
                    lines.push_back(line);
                }
            }
            catch (const FileParseException& e) {
                if (std::string(e.what()).find("End of file") != std::string::npos) {
                    break;
                }
                continue;
            }
        }

        analoguesFile.Close_file_in();

        // Перезаписываем файл
        if (!analoguesFile.Open_file_trunc()) {
            return false;
        }

        for (const auto& savedLine : lines) {
            analoguesFile.Write_string_line(savedLine);
        }

        analoguesFile.Flush();
        analoguesFile.Close_file_o();
        return true;
    }
    catch (const std::exception& e) {
        analoguesFile.Close_file_in();
        analoguesFile.Close_file_o();
        return false;
    }
}

// Получение аналогов для конкретного лекарства
std::vector<std::string> FileManager::getMedicineAnalogues(const std::string& medicineId)
{
    std::vector<std::string> analogues;

    try {
        analoguesFile.Close_file_in();

        if (!analoguesFile.Open_file_in()) {
            return analogues; // Файл не существует
        }

        std::string line;
        while (true) {
            try {
                analoguesFile.Read_string_line(line);

                size_t delimiterPos = line.find(';');
                if (delimiterPos != std::string::npos) {
                    std::string currentMedicineId = line.substr(0, delimiterPos);
                    std::string analogueId = line.substr(delimiterPos + 1);

                    if (currentMedicineId == medicineId) {
                        analogues.push_back(analogueId);
                    }
                }
            }
            catch (const FileParseException& e) {
                if (std::string(e.what()).find("End of file") != std::string::npos) {
                    break;
                }
                continue;
            }
        }

        analoguesFile.Close_file_in();
    }
    catch (const std::exception& e) {
        analoguesFile.Close_file_in();
    }

    return analogues;
}
