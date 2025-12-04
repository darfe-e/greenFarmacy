#include "supply.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/InventoryExceptions/NegativeQuantityException.h"
#include "Exception/safeinput.h"
#include <sstream>
#include <stdexcept>

Supply::Supply(std::string id, SafeDate date,
               std::shared_ptr<MedicalProduct> product, int quantity,
               std::string src, std::string dest, std::string status)
    : InventoryOperation(std::move(id), date, std::move(product), quantity, std::move(status))
    , source(std::move(src))
    , destination(std::move(dest))
{
    if (source.empty())
        throw InventoryException("Supply source cannot be empty");
    if (destination.empty())
        throw InventoryException("Supply destination cannot be empty");
}

Supply::Supply()
    : InventoryOperation()
    , source("")
    , destination("")
{
}

Supply::Supply(const Supply& other)
    : InventoryOperation(other)
    , source(other.source)
    , destination(other.destination)
{
}

std::string Supply::getOperationType() const
{
    return "Supply";
}

void Supply::process()
{
    if (getStatus() == "completed")
        throw InventoryException("Supply operation already completed");

    if (getStatus() == "cancelled")
        throw InventoryException("Supply operation is cancelled");

    if (getQuantity() <= 0)
        throw NegativeQuantityException(getQuantity());

    setStatus("completed");
}

Supply& Supply::operator=(const Supply& other)
{
    if (this != &other)
    {
        InventoryOperation::operator=(other);
        source = other.source;
        destination = other.destination;
    }
    return *this;
}

// Вывод в файл
std::ostream& operator<<(std::ostream& os, const Supply& supply)
{
    os << supply.id << ";"
       << supply.operationDate.toString() << ";"
       << supply.product->getId() << ";"
       << supply.quantity << ";"
       << supply.status << ";"
       << supply.source << ";"
       << supply.destination;
    return os;
}

// Ввод из файла
std::istream& operator>>(std::istream& is, Supply& supply)
{
    std::string line;
    if (!std::getline(is, line) || line.empty()) {
        is.setstate(std::ios::failbit);
        return is;
    }

    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, ';')) {
        tokens.push_back(token);
    }

    // Для Supply должно быть 7 полей
    if (tokens.size() < 7) {
        throw InventoryException("Invalid number of fields for supply operation");
    }

    try {
        // Заполняем поля напрямую
        supply.id = tokens[0];
        supply.operationDate = SafeDate::fromString(tokens[1]);

        // Создаем временный продукт
        supply.product = std::make_shared<MedicalProduct>(
            tokens[2], "Temp Product", 0.0, SafeDate(), "Unknown");

        supply.quantity = std::stoi(tokens[3]);
        supply.status = tokens[4];
        supply.source = tokens[5];
        supply.destination = tokens[6];

        if (supply.source.empty() || supply.destination.empty()) {
            throw InventoryException("Supply source and destination cannot be empty");
        }
    }
    catch (const std::exception& e) {
        throw InventoryException("Error reading supply operation: " + std::string(e.what()));
    }

    return is;
}
