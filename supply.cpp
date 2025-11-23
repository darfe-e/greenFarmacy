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
    os << static_cast<const InventoryOperation&>(supply) << ";"
       << supply.source << ";"
       << supply.destination;

    return os;
}

std::istream& operator>>(std::istream& is, Supply& supply)
{
    try
    {
        std::string line;
        std::getline(is, line);

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ';')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 3) {
            throw InventoryException("Invalid number of fields for supply operation");
        }

        // Восстанавливаем базовую операцию
        std::istringstream baseStream(tokens[0]);
        baseStream >> static_cast<InventoryOperation&>(supply);

        // Source и Destination
        supply.source = tokens[1];
        if (supply.source.empty()) {
            throw InventoryException("Supply source cannot be empty");
        }

        supply.destination = tokens[2];
        if (supply.destination.empty()) {
            throw InventoryException("Supply destination cannot be empty");
        }

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
