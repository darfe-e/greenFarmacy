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

// Вывод в файл (компактный формат)
std::ostream& operator<<(std::ostream& os, const InventoryOperation& operation)
{
    os << operation.id << ";"
       << operation.operationDate.toString() << ";"
       << operation.product->getId() << ";"
       << operation.quantity << ";"
       << operation.status;
    return os;
}

// Ввод из файла (компактный формат)
std::istream& operator>>(std::istream& is, InventoryOperation& operation)
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

    // Должно быть 5 полей: id;date;productId;quantity;status
    if (tokens.size() < 5) {
        is.setstate(std::ios::failbit);
        return is;
    }

    try {
        operation.id = tokens[0];
        operation.operationDate = SafeDate::fromString(tokens[1]);

        // Создаем временный продукт - потом заменится на реальный
        operation.product = std::make_shared<MedicalProduct>(
            tokens[2], "Temp Product", 0.0, SafeDate(), "Unknown");

        operation.quantity = std::stoi(tokens[3]);
        operation.status = tokens[4];
    }
    catch (const std::exception& e) {
        is.setstate(std::ios::failbit);
        throw InventoryException("Error reading inventory operation: " + std::string(e.what()));
    }

    return is;
}
