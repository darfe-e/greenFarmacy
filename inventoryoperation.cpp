#include "inventoryoperation.h"
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
        throw std::invalid_argument("Operation ID cannot be empty");
    if (!this->product)
        throw std::invalid_argument("Product cannot be null");
    if (this->quantity <= 0)
        throw std::invalid_argument("Quantity must be positive: " + std::to_string(this->quantity));
    if (this->status.empty())
        throw std::invalid_argument("Status cannot be empty");
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
        std::string temp;

        // Чтение ID операции
        std::getline(is, temp);                    // "Operation ID: "
        std::getline(is, operation.id);

        // Чтение и парсинг даты
        std::getline(is, temp);                    // "Date: "
        std::getline(is, temp);

        // Парсинг даты из формата "YYYY-MM-DD"
        std::stringstream dateSS(temp);
        std::string yearStr, monthStr, dayStr;

        std::getline(dateSS, yearStr, '-');
        std::getline(dateSS, monthStr, '-');
        std::getline(dateSS, dayStr, '-');

        int year = std::stoi(yearStr);
        int month = std::stoi(monthStr);
        int day = std::stoi(dayStr);

        operation.operationDate = SafeDate(year, month, day);

        // Чтение информации о продукте
        std::getline(is, temp);                    // "Product: "
        std::getline(is, temp);

        std::string productId = temp; // предполагаем, что это ID продукта
        std::string productName = "Temp Product"; // временное имя

        // Создаем временный продукт (в реальной системе нужно искать в каталоге)
        operation.product = std::make_shared<MedicalProduct>(
            productId, productName, 0.0, SafeDate(2025, 12, 31), "Unknown");

        // Чтение количества
        std::getline(is, temp);                    // "Quantity: "
        std::getline(is, temp);
        operation.quantity = std::stoi(temp);

        // Чтение статуса
        std::getline(is, temp);                    // "Status: "
        std::getline(is, operation.status);

        // Пропускаем тип операции (он будет обработан в производных классах)
        std::getline(is, temp);                    // "Type: "
        std::getline(is, temp);

    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading InventoryOperation data: " + std::string(e.what()));
    }

    return is;
}
