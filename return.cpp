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
    os << static_cast<const InventoryOperation&>(returnOp);
    os << "Reason: " << returnOp.reason << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Return& returnOp)
{
    try
    {
        is >> static_cast<InventoryOperation&>(returnOp);

        SafeInput::skipLabel(is); // "Reason: "
        returnOp.reason = SafeInput::readNonEmptyString(is, "Return reason");

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
