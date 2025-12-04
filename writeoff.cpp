#include "writeoff.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/InventoryExceptions/NegativeQuantityException.h"
#include "Exception/safeinput.h"
#include <sstream>
#include <stdexcept>

WriteOff::WriteOff(std::string id, SafeDate date,
                   std::shared_ptr<MedicalProduct> product, int quantity,
                   std::string reason, std::string status)
    : InventoryOperation(std::move(id), date, std::move(product), quantity, std::move(status))
    , writeOffReason(std::move(reason))
{
    if (writeOffReason.empty())
        throw InventoryException("Write-off reason cannot be empty");
}

WriteOff::WriteOff()
    : InventoryOperation()
    , writeOffReason("")
{
}

WriteOff::WriteOff(const WriteOff& other)
    : InventoryOperation(other)
    , writeOffReason(other.writeOffReason)
{
}

std::string WriteOff::getOperationType() const
{
    return "WriteOff";
}

void WriteOff::process()
{
    if (getStatus() == "completed")
        throw InventoryException("Write-off operation already completed");

    if (getStatus() == "cancelled")
        throw InventoryException("Write-off operation is cancelled");

    if (getQuantity() <= 0)
        throw NegativeQuantityException(getQuantity());

    setStatus("completed");
}

WriteOff& WriteOff::operator=(const WriteOff& other)
{
    if (this != &other)
    {
        InventoryOperation::operator=(other);
        writeOffReason = other.writeOffReason;
    }
    return *this;
}

// Вывод в файл
std::ostream& operator<<(std::ostream& os, const WriteOff& writeOff)
{
    os << writeOff.id << ";"
       << writeOff.operationDate.toString() << ";"
       << writeOff.product->getId() << ";"
       << writeOff.quantity << ";"
       << writeOff.status << ";"
       << writeOff.writeOffReason;
    return os;
}

// Ввод из файла
std::istream& operator>>(std::istream& is, WriteOff& writeOff)
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

    // Для WriteOff должно быть 6 полей
    if (tokens.size() < 6) {
        throw InventoryException("Invalid number of fields for write-off operation");
    }

    try {
        // Заполняем поля напрямую
        writeOff.id = tokens[0];
        writeOff.operationDate = SafeDate::fromString(tokens[1]);

        // Создаем временный продукт
        writeOff.product = std::make_shared<MedicalProduct>(
            tokens[2], "Temp Product", 0.0, SafeDate(), "Unknown");

        writeOff.quantity = std::stoi(tokens[3]);
        writeOff.status = tokens[4];
        writeOff.writeOffReason = tokens[5];

        if (writeOff.writeOffReason.empty()) {
            throw InventoryException("Write-off reason cannot be empty");
        }
    }
    catch (const std::exception& e) {
        throw InventoryException("Error reading write-off operation: " + std::string(e.what()));
    }

    return is;
}
