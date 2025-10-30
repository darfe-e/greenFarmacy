#include "writeoff.h"
#include <sstream>
#include <stdexcept>

WriteOff::WriteOff(std::string id, SafeDate date,
                   std::shared_ptr<MedicalProduct> product, int quantity,
                   std::string reason, std::string status)
    : InventoryOperation(std::move(id), date, std::move(product), quantity, std::move(status))
    , writeOffReason(std::move(reason))
{
    if (writeOffReason.empty())
        throw std::invalid_argument("Write-off reason cannot be empty");
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
        throw std::runtime_error("Write-off operation already completed");

    if (getStatus() == "cancelled")
        throw std::runtime_error("Write-off operation is cancelled");

    if (getQuantity() <= 0)
        throw std::runtime_error("Write-off quantity must be positive");

    setStatus("completed");                       // Помечаем как завершенную
}

WriteOff& WriteOff::operator=(const WriteOff& other)
{
    if (this != &other)
    {
        InventoryOperation::operator=(other);     // Присваиваем базовую часть
        writeOffReason = other.writeOffReason;    // Присваиваем причину списания
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const WriteOff& writeOff)
{
    os << static_cast<const InventoryOperation&>(writeOff);  // Выводим базовую часть
    os << "Write-off Reason: " << writeOff.writeOffReason << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, WriteOff& writeOff)
{
    try
    {
        is >> static_cast<InventoryOperation&>(writeOff);   // Читаем базовую часть

        std::string temp;
        std::getline(is, temp);                    // Читаем текущую строку

        if (temp.find("Write-off Reason:") != std::string::npos)
        {
            writeOff.writeOffReason = temp.substr(temp.find("Write-off Reason:") + 17);
            size_t start = writeOff.writeOffReason.find_first_not_of(" ");
            if (start != std::string::npos)
                writeOff.writeOffReason = writeOff.writeOffReason.substr(start);
        }
        else
        {
            std::getline(is, writeOff.writeOffReason);
        }

        if (writeOff.writeOffReason.empty())
            throw std::invalid_argument("Write-off reason cannot be empty");
    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading WriteOff data: " + std::string(e.what()));
    }

    return is;
}
