#include "supply.h"
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
        throw std::invalid_argument("Supply source cannot be empty");
    if (destination.empty())
        throw std::invalid_argument("Supply destination cannot be empty");
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
        throw std::runtime_error("Supply operation already completed");

    if (getStatus() == "cancelled")
        throw std::runtime_error("Supply operation is cancelled");

    if (getQuantity() <= 0)
        throw std::runtime_error("Supply quantity must be positive");

    setStatus("completed");                       // Помечаем как завершенную
}

Supply& Supply::operator=(const Supply& other)
{
    if (this != &other)
    {
        InventoryOperation::operator=(other);     // Присваиваем базовую часть
        source = other.source;                    // Присваиваем источник
        destination = other.destination;          // Присваиваем назначение
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Supply& supply)
{
    os << static_cast<const InventoryOperation&>(supply);  // Выводим базовую часть
    os << "Source: " << supply.source << "\n";             // Выводим источник
    os << "Destination: " << supply.destination << "\n";   // Выводим назначение
    return os;
}

std::istream& operator>>(std::istream& is, Supply& supply)
{
    try
    {
        is >> static_cast<InventoryOperation&>(supply);    // Читаем базовую часть

        std::string temp;

        std::getline(is, temp);                    // "Source: "
        if (temp.find("Source:") != std::string::npos)
            supply.source = temp.substr(temp.find("Source:") + 7);
        else
            std::getline(is, supply.source);

        std::getline(is, temp);                    // "Destination: "
        if (temp.find("Destination:") != std::string::npos)
            supply.destination = temp.substr(temp.find("Destination:") + 12);
        else
            std::getline(is, supply.destination);

        if (supply.source.empty())
            throw std::invalid_argument("Supply source cannot be empty");
        if (supply.destination.empty())
            throw std::invalid_argument("Supply destination cannot be empty");
    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading Supply data: " + std::string(e.what()));
    }

    return is;
}
