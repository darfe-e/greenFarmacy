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

FileManager::FileManager()                                                     // Конструктор FileManager
    : medicinesFile("medicines.txt")
    , pharmaciesFile("pharmacies.txt")
    , inventoryOperationsFile("operations.txt")
    , stockFile("stock.txt")
    , analoguesFile("analogues.txt")
{
}

FileManager& FileManager::getInstance()                                        // Получение экземпляра FileManager
{
    if (instance == nullptr)
        instance = new FileManager();
    return *instance;
}

bool FileManager::loadMedicines(std::vector<std::shared_ptr<Medicine>>& medicines) // Загрузка лекарств
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

                if (line.find("[TABLET];") == 0)                              // Загрузка таблеток
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
                else if (line.find("[SYRUP];") == 0)                          // Загрузка сиропа
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
                else if (line.find("[OINTMENT];") == 0)                       // Загрузка мази
                {
                    Ointment ointment;
                    std::string remainingLine = line.substr(11);
                    std::istringstream ointmentStream(remainingLine);
                    ointmentStream >> ointment;
                    if (!ointmentStream.fail())
                        medicines.push_back(std::make_shared<Ointment>(ointment));
                    else
                        throw FileParseException("Failed to parse ointment data");
                }
                else
                    throw FileParseException("Unknown medicine type marker");

            }
            catch (const FileParseException& e)
            {
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
        if (!loadAnalogues(medicines))                                        // Загрузка аналогов
        {
        }
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

bool FileManager::saveMedicines(const std::vector<std::shared_ptr<Medicine>>& medicines) // Сохранение лекарств
{
    try
    {
        medicinesFile.Close_file_o();

        if (!medicinesFile.Open_file_trunc())
            throw FileWriteException("Failed to open medicines.txt for writing");

        for (const auto& med : medicines)
        {
            if (med)
            {
                try
                {
                    if (auto tablet = std::dynamic_pointer_cast<Tablet>(med)) // Сохранение таблеток
                    {
                        Tablet tempTablet = *tablet;
                        std::ostringstream oss;
                        oss << tempTablet;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                    }
                    else if (auto syrup = std::dynamic_pointer_cast<Syrup>(med)) // Сохранение сиропа
                    {
                        Syrup tempSyrup = *syrup;
                        std::ostringstream oss;
                        oss << tempSyrup;
                        std::string line = oss.str();
                        medicinesFile.Write_string_line(line);
                    }
                    else if (auto ointment = std::dynamic_pointer_cast<Ointment>(med)) // Сохранение мази
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

// bool FileManager::addMedicine(std::shared_ptr<Medicine> medicine)              // Добавление одного лекарства
// {
//     try
//     {
//         if (!medicine) return false;

//         SafeInput::validateProductId(medicine->getId());                      // Валидация ID
//         SafeInput::validateTextField(medicine->getName(), "Name");            // Валидация названия
//         SafeInput::validateTextField(medicine->getManufacturerCountry(), "Country"); // Валидация страны
//         SafeInput::validateTextField(medicine->getActiveSubstance(), "Active substance"); // Валидация вещества

//         if (medicine->getBasePrice() <= 0)                                    // Проверка цены
//             return false;

//         medicinesFile.Close_file_o();
//         if (!medicinesFile.Open_file_out()) return false;

//         if (auto tablet = std::dynamic_pointer_cast<Tablet>(medicine))        // Добавление таблеток
//         {
//             std::ostringstream oss;
//             oss << *tablet;
//             medicinesFile.Write_string_line(oss.str());
//         }
//         else if (auto syrup = std::dynamic_pointer_cast<Syrup>(medicine))     // Добавление сиропа
//         {
//             std::ostringstream oss;
//             oss << *syrup;
//             medicinesFile.Write_string_line(oss.str());
//         }
//         else if (auto ointment = std::dynamic_pointer_cast<Ointment>(medicine)) // Добавление мази
//         {
//             std::ostringstream oss;
//             oss << *ointment;
//             medicinesFile.Write_string_line(oss.str());
//         }
//         else
//             return false;

//         medicinesFile.Flush();
//         medicinesFile.Close_file_o();
//         return true;

//     }
//     catch (...)
//     {
//         medicinesFile.Close_file_o();
//         return false;
//     }
// }

bool FileManager::loadPharmacies(std::vector<std::shared_ptr<Pharmacy>>& pharmacies) // Загрузка аптек
{
    try
    {
        pharmacies.clear();

        std::ifstream file("pharmacies.txt");                                 // Чтение из файла
        if (file.is_open())
        {
            std::string line;
            int lineNum = 0;

            while (std::getline(file, line))
            {
                lineNum++;

                if (line.empty() || line.find_first_not_of(' ') == std::string::npos)
                    continue;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::string token;

                while (std::getline(iss, token, ';'))                         // Разделение строки по ';'
                    tokens.push_back(token);

                if (tokens.size() == 5)                                       // Проверка количества полей
                {
                    try
                    {
                        std::string id = tokens[0];
                        std::string name = tokens[1];
                        std::string address = tokens[2];
                        std::string phone = tokens[3];

                        std::istringstream rentStream(tokens[4]);             // Парсинг аренды
                        double rent;
                        if (!(rentStream >> rent))
                            continue;

                        pharmacies.push_back(std::make_shared<Pharmacy>(id, name, address, phone, rent));
                    }
                    catch (const std::exception& e)
                    {
                        continue;
                    }
                }
            }

            file.close();

            if (!pharmacies.empty())
                return true;
        }

        pharmacies.push_back(std::make_shared<Pharmacy>("001", "Главная аптека", "ул. Ленина, 10", "+7-495-123-4567", 50000)); // Тестовые данные
        pharmacies.push_back(std::make_shared<Pharmacy>("002", "Аптека №2", "пр. Мира, 25", "+7-495-765-4321", 35000));
        pharmacies.push_back(std::make_shared<Pharmacy>("003", "Северная аптека", "ул. Северная, 5", "+7-495-555-8899", 28000));
        pharmacies.push_back(std::make_shared<Pharmacy>("004", "Западная аптека", "ул. Западная, 15", "+7-495-777-8888", 42000));

        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::savePharmacies(const std::vector<std::shared_ptr<Pharmacy>>& pharmacies) // Сохранение аптек
{
    try
    {
        if (!pharmaciesFile.Open_file_out()) return false;

        for (const auto& pharmacy : pharmacies)                               // Запись каждой аптеки
        {
            if (pharmacy)
            {
                Pharmacy nonConstPharmacy = *pharmacy;
                pharmaciesFile.Write_record_in_file_text(nonConstPharmacy);
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::loadStockData(std::vector<std::shared_ptr<Pharmacy>>& pharmacies, const std::vector<std::shared_ptr<Medicine>>& medicines) // Загрузка данных о запасах
{
    try
    {
        if (!stockFile.Open_file_in()) return false;

        std::map<std::string, std::shared_ptr<Medicine>> medicineMap;         // Карта лекарств по ID
        for (const auto& med : medicines)
        {
            if (med)
                medicineMap[med->getId()] = med;
        }

        std::map<std::string, std::shared_ptr<Pharmacy>> pharmacyMap;        // Карта аптек по ID
        for (auto& pharmacy : pharmacies)
            pharmacyMap[pharmacy->getId()] = pharmacy;

        StockRecord record;
        while (!stockFile.R_end_file())
        {
            stockFile.Read_record_in_file_text(record);
            if (!stockFile.R_end_file())
            {
                auto pharmacyIt = pharmacyMap.find(record.pharmacyId);       // Поиск аптеки
                auto medicineIt = medicineMap.find(record.productId);        // Поиск лекарства

                if (pharmacyIt != pharmacyMap.end() && medicineIt != medicineMap.end())
                    pharmacyIt->second->addToStorage(medicineIt->second, record.quantity); // Добавление в склад
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::saveStockData(const std::vector<std::shared_ptr<Pharmacy>>& pharmacies) // Сохранение данных о запасах
{
    try
    {
        if (!stockFile.Open_file_out()) return false;

        for (const auto& pharmacy : pharmacies)
        {
            if (!pharmacy) continue;

            auto products = pharmacy->getAllProducts();                      // Получение всех продуктов
            for (const auto& productPair : products)
            {
                const auto& product = productPair.first;
                int quantity = productPair.second;

                if (product && quantity > 0)
                {
                    std::time_t t = std::time(nullptr);
                    std::tm* now = std::localtime(&t);
                    SafeDate currentDate(*now);                               // Текущая дата

                    StockRecord record(product->getId(), pharmacy->getId(), quantity, currentDate);
                    stockFile.Write_record_in_file_text(record);             // Запись записи о запасе
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


bool FileManager::loadInventoryOperations(std::vector<std::shared_ptr<InventoryOperation>>& operations) // Загрузка операций
{
    try
    {
        if (!inventoryOperationsFile.Open_file_in()) return false;

        operations.clear();

        while (!inventoryOperationsFile.R_end_file())
        {
            try
            {
                std::string line;
                inventoryOperationsFile.Read_string_line(line);

                if (line.empty()) continue;

                if (line.find("SUPPLY;") == 0)                               // Загрузка поставок
                {
                    std::string data = line.substr(7);
                    std::istringstream iss(data);

                    Supply supply;
                    iss >> supply;
                    if (!iss.fail())
                        operations.push_back(std::make_shared<Supply>(supply));
                }
                else if (line.find("RETURN;") == 0)                          // Загрузка возвратов
                {
                    std::string data = line.substr(7);
                    std::istringstream iss(data);

                    Return returnOp;
                    iss >> returnOp;
                    if (!iss.fail())
                        operations.push_back(std::make_shared<Return>(returnOp));
                }
                else if (line.find("WRITEOFF;") == 0)                        // Загрузка списаний
                {
                    std::string data = line.substr(9);
                    std::istringstream iss(data);

                    WriteOff writeOff;
                    iss >> writeOff;
                    if (!iss.fail())
                        operations.push_back(std::make_shared<WriteOff>(writeOff));
                }
            }
            catch (const FileParseException& e)                              // Конец файла
            {
                break;
            }
            catch (const std::exception& e)                                  // Обработка ошибок
            {
                std::string error = e.what();
                if (error.find("Product") != std::string::npos &&
                    error.find("is expired") != std::string::npos)           // Игнорирование просроченных
                    continue;
                continue;
            }
        }

        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

bool FileManager::saveInventoryOperations(const std::vector<std::shared_ptr<InventoryOperation>>& operations) // Сохранение операций
{
    try
    {
        std::vector<std::shared_ptr<InventoryOperation>> allOperations;      // Все операции
        loadInventoryOperations(allOperations);                              // Загрузка существующих

        for (const auto& newOp : operations)                                 // Добавление новых операций
        {
            if (!newOp) continue;

            bool exists = false;
            for (const auto& existingOp : allOperations)                     // Проверка на существование
            {
                if (existingOp && existingOp->getId() == newOp->getId())
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
                allOperations.push_back(newOp);                              // Добавление новой операции
        }

        if (!inventoryOperationsFile.Open_file_trunc()) return false;       // Перезапись файла

        for (const auto& operation : allOperations)                          // Запись всех операций
        {
            if (operation)
            {
                if (auto supply = std::dynamic_pointer_cast<Supply>(operation))
                {
                    std::ostringstream oss;
                    oss << "SUPPLY;" << *supply;
                    inventoryOperationsFile.Write_string_line(oss.str());
                }
                else if (auto returnOp = std::dynamic_pointer_cast<Return>(operation))
                {
                    std::ostringstream oss;
                    oss << "RETURN;" << *returnOp;
                    inventoryOperationsFile.Write_string_line(oss.str());
                }
                else if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(operation))
                {
                    std::ostringstream oss;
                    oss << "WRITEOFF;" << *writeOff;
                    inventoryOperationsFile.Write_string_line(oss.str());
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

bool FileManager::loadAnalogues(std::vector<std::shared_ptr<Medicine>>& medicines) // Загрузка аналогов
{
    try
    {
        analoguesFile.Close_file_in();

        if (!analoguesFile.Open_file_in())                                   // Файл может не существовать
            return true;

        std::map<std::string, std::shared_ptr<Medicine>> medicineMap;        // Карта лекарств
        for (const auto& med : medicines)
        {
            if (med)
                medicineMap[med->getId()] = med;
        }

        std::string line;
        while (true)
        {
            try
            {
                analoguesFile.Read_string_line(line);

                size_t delimiterPos = line.find(';');                        // Разделитель ';'
                if (delimiterPos != std::string::npos)
                {
                    std::string medicineId = line.substr(0, delimiterPos);   // ID лекарства
                    std::string analogueId = line.substr(delimiterPos + 1);  // ID аналога

                    auto medicineIt = medicineMap.find(medicineId);         // Поиск лекарства
                    auto analogueIt = medicineMap.find(analogueId);         // Поиск аналога

                    if (medicineIt != medicineMap.end() && analogueIt != medicineMap.end())
                        medicineIt->second->addAnalogue(analogueIt->second); // Добавление аналога
                }
            }
            catch (const FileParseException& e)
            {
                if (std::string(e.what()).find("End of file") != std::string::npos)
                    break;
                continue;
            }
        }

        analoguesFile.Close_file_in();
        return true;
    }
    catch (const std::exception& e)
    {
        analoguesFile.Close_file_in();
        return false;
    }
}

bool FileManager::saveAnalogues(const std::vector<std::shared_ptr<Medicine>>& medicines) // Сохранение аналогов
{
    try
    {
        analoguesFile.Close_file_o();

        if (!analoguesFile.Open_file_trunc())
            return false;

        for (const auto& medicine : medicines)
        {
            if (medicine)
            {
                auto analogueIds = medicine->getAnalogueIds();              // Получение ID аналогов
                for (const auto& analogueId : analogueIds)
                {
                    std::string line = medicine->getId() + ";" + analogueId; // Формат: medicineId;analogueId
                    analoguesFile.Write_string_line(line);                  // Запись строки
                }
            }
        }

        analoguesFile.Flush();
        analoguesFile.Close_file_o();
        return true;
    }
    catch (const std::exception& e)
    {
        analoguesFile.Close_file_o();
        return false;
    }
}

std::vector<std::pair<std::string, int>> FileManager::getAvailabilityInOtherPharmacies(
    const std::string& productId)
{
    std::vector<std::pair<std::string, int>> result;

    try
    {

        std::vector<std::shared_ptr<Pharmacy>> allPharmacies;
        if (!loadPharmacies(allPharmacies))
            return result;

        std::ifstream stockFile("stock.txt");
        if (!stockFile.is_open())
            return result;

        std::string mainPharmacyId = "001";

        std::string line;
        int lineNumber = 0;
        int foundMatches = 0;

        while (std::getline(stockFile, line))
        {
            lineNumber++;

            if (line.empty() || line.find_first_not_of(' ') == std::string::npos)
                continue;

            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos)
                continue;

            std::string prodId = line.substr(0, colonPos);
            std::string rest = line.substr(colonPos + 1);

            prodId.erase(std::remove(prodId.begin(), prodId.end(), ' '), prodId.end());

            std::stringstream ss(rest);
            std::string pharmId, quantityStr, dateStr;

            if (std::getline(ss, pharmId, ';') &&
                std::getline(ss, quantityStr, ';'))
            {

                pharmId.erase(std::remove(pharmId.begin(), pharmId.end(), ' '), pharmId.end());
                quantityStr.erase(std::remove(quantityStr.begin(), quantityStr.end(), ' '), quantityStr.end());

                bool prodIdIsValid = !prodId.empty() &&
                                     std::all_of(prodId.begin(), prodId.end(), ::isdigit);
                bool pharmIdIsValid = !pharmId.empty() &&
                                      std::all_of(pharmId.begin(), pharmId.end(), ::isdigit);

                if (!prodIdIsValid || !pharmIdIsValid)
                    continue;

                bool productMatches = (prodId == productId);
                bool notMainPharmacy = (pharmId != mainPharmacyId);

                if (productMatches && notMainPharmacy)
                {

                    try
                    {
                        int quantity = std::stoi(quantityStr);
                        if (quantity > 0)
                        {
                            bool foundPharmacy = false;
                            for (const auto& pharmacy : allPharmacies)
                            {
                                if (!pharmacy) continue;

                                std::string pharmacyId = pharmacy->getId();

                                if (pharmacyId == pharmId)
                                {
                                    std::string displayName = pharmacy->getName() + "\n" +
                                                              pharmacy->getAddress() + "\n" +
                                                              pharmacy->getPhoneNumber();
                                    result.push_back({displayName, quantity});
                                    foundPharmacy = true;
                                    foundMatches++;

                                    break;
                                }
                            }

                            if (!foundPharmacy)
                            {

                                result.push_back({"Аптека №" + pharmId, quantity});
                                foundMatches++;
                            }
                        }
                    }
                    catch (const std::exception& e)
                    {
                        qWarning() << "Ошибка преобразования количества: " << quantityStr.c_str();
                    }
                }
            }
        }

        stockFile.close();

    }
    catch (const std::exception& e)
    {
        qWarning() << "Ошибка в getAvailabilityInOtherPharmacies: " << e.what();
    }

    return result;
}
