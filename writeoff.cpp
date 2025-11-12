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

std::ostream& operator<<(std::ostream& os, const WriteOff& writeOff)
{
    os << static_cast<const InventoryOperation&>(writeOff);
    os << "Write-off Reason: " << writeOff.writeOffReason << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, WriteOff& writeOff)
{
    try
    {
        is >> static_cast<InventoryOperation&>(writeOff);

        SafeInput::skipLabel(is); // "Write-off Reason: "
        writeOff.writeOffReason = SafeInput::readNonEmptyString(is, "Write-off reason");

    }
    catch (const InventoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InventoryException("Error reading write-off data: " + std::string(e.what()));
    }

    return is;
}
