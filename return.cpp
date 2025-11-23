#include "return.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/safeinput.h"
#include <sstream>
#include <stdexcept>

Return::Return(std::string id, SafeDate date,
               std::shared_ptr<MedicalProduct> product, int quantity,
               std::string reason, std::string status)
    : InventoryOperation(std::move(id), date, std::move(product), quantity, std::move(status))
    , reason(std::move(reason))
{
    if (this->reason.empty())
        throw InventoryException("Return reason cannot be empty");
}

Return::Return()
    : InventoryOperation()
    , reason("")
{
}

Return::Return(const Return& other)
    : InventoryOperation(other)
    , reason(other.reason)
{
}

std::string Return::getOperationType() const
{
    return "Return";
}

void Return::process()
{
    if (getStatus() == "completed")
        throw InventoryException("Return operation already completed");

    if (getStatus() == "cancelled")
        throw InventoryException("Return operation is cancelled");

    setStatus("completed");
}

Return& Return::operator=(const Return& other)
{
    if (this != &other)
    {
        InventoryOperation::operator=(other);
        reason = other.reason;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Return& returnOp)
{
    os << static_cast<const InventoryOperation&>(returnOp) << ";"
       << returnOp.reason;

    return os;
}

std::istream& operator>>(std::istream& is, Return& returnOp)
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

        if (tokens.size() < 2) {
            throw InventoryException("Invalid number of fields for return operation");
        }

        // Восстанавливаем базовую операцию
        std::istringstream baseStream(tokens[0]);
        baseStream >> static_cast<InventoryOperation&>(returnOp);

        // Reason
        returnOp.reason = tokens[1];
        if (returnOp.reason.empty()) {
            throw InventoryException("Return reason cannot be empty");
        }

    }
    catch (const InventoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InventoryException("Error reading return data: " + std::string(e.what()));
    }

    return is;
}
