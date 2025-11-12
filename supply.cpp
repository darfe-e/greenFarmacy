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

std::ostream& operator<<(std::ostream& os, const Supply& supply)
{
    os << static_cast<const InventoryOperation&>(supply);
    os << "Source: " << supply.source << "\n";
    os << "Destination: " << supply.destination << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Supply& supply)
{
    try
    {
        is >> static_cast<InventoryOperation&>(supply);

        SafeInput::skipLabel(is); // "Source: "
        supply.source = SafeInput::readNonEmptyString(is, "Supply source");

        SafeInput::skipLabel(is); // "Destination: "
        supply.destination = SafeInput::readNonEmptyString(is, "Supply destination");

    }
    catch (const InventoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InventoryException("Error reading supply data: " + std::string(e.what()));
    }

    return is;
}
