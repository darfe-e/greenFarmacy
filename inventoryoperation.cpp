#include "inventoryoperation.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/InventoryExceptions/NegativeQuantityException.h"
#include "Exception/safeinput.h"
#include <sstream>
#include <stdexcept>

InventoryOperation::InventoryOperation(std::string id, SafeDate date,
                                       std::shared_ptr<MedicalProduct> product, int quantity,
                                       std::string status)
    : id(std::move(id))
    , operationDate(date)
    , product(std::move(product))
    , quantity(quantity)
    , status(std::move(status))
{
    if (this->id.empty())
        throw InventoryException("Operation ID cannot be empty");
    if (!this->product)
        throw InventoryException("Product cannot be null");
    if (this->quantity <= 0)
        throw NegativeQuantityException(this->quantity);
    if (this->status.empty())
        throw InventoryException("Status cannot be empty");
}

InventoryOperation::InventoryOperation()
    : id("")
    , operationDate(SafeDate(2000, 1, 1))
    , product(nullptr)
    , quantity(0)
    , status("pending")
{
}

InventoryOperation::InventoryOperation(const InventoryOperation& other)
    : id(other.id)
    , operationDate(other.operationDate)
    , product(other.product)
    , quantity(other.quantity)
    , status(other.status)
{
}

InventoryOperation& InventoryOperation::operator=(const InventoryOperation& other)
{
    if (this != &other)
    {
        id = other.id;
        operationDate = other.operationDate;
        product = other.product;
        quantity = other.quantity;
        status = other.status;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const InventoryOperation& operation)
{
    os << "Operation ID: " << operation.id << "\n"
       << "Date: " << operation.operationDate.toString() << "\n"
       << "Product: " << operation.product->getName() << " (" << operation.product->getId() << ")\n"
       << "Quantity: " << operation.quantity << "\n"
       << "Status: " << operation.status << "\n"
       << "Type: " << operation.getOperationType() << "\n";

    return os;
}

std::istream& operator>>(std::istream& is, InventoryOperation& operation)
{
    try
    {
        SafeInput::skipLabel(is); // "Operation ID: "
        operation.id = SafeInput::readNonEmptyString(is, "Operation ID");

        SafeInput::skipLabel(is); // "Date: "
        std::string dateStr = SafeInput::readNonEmptyString(is, "Date");
        operation.operationDate = SafeDate::fromString(dateStr);

        SafeInput::skipLabel(is); // "Product: "
        std::string productInfo = SafeInput::readNonEmptyString(is, "Product");

        // Временный продукт (в реальной системе нужно искать в каталоге)
        operation.product = std::make_shared<MedicalProduct>(
            "temp_id", "Temp Product", 0.0, SafeDate(2025, 12, 31), "Unknown");

        SafeInput::skipLabel(is); // "Quantity: "
        operation.quantity = SafeInput::readPositiveInt(is, "Quantity");

        SafeInput::skipLabel(is); // "Status: "
        operation.status = SafeInput::readNonEmptyString(is, "Status");

        SafeInput::skipLabel(is); // "Type: "
        std::string type = SafeInput::readNonEmptyString(is, "Operation type");

    }
    catch (const InventoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InventoryException("Error reading inventory operation: " + std::string(e.what()));
    }

    return is;
}
