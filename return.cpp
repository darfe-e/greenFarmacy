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
    os << returnOp.id << ";"
       << returnOp.operationDate.toString() << ";"
       << returnOp.product->getId() << ";"
       << returnOp.quantity << ";"
       << returnOp.status << ";"
       << returnOp.reason;
    return os;
}

// Ввод из файла
std::istream& operator>>(std::istream& is, Return& returnOp)
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

    // Для Return должно быть 6 полей
    if (tokens.size() < 6) {
        throw InventoryException("Invalid number of fields for return operation");
    }

    try {
        // Заполняем поля напрямую
        returnOp.id = tokens[0];
        returnOp.operationDate = SafeDate::fromString(tokens[1]);

        // Создаем временный продукт
        returnOp.product = std::make_shared<MedicalProduct>(
            tokens[2], "Temp Product", 0.0, SafeDate(), "Unknown");

        returnOp.quantity = std::stoi(tokens[3]);
        returnOp.status = tokens[4];
        returnOp.reason = tokens[5];

        if (returnOp.reason.empty()) {
            throw InventoryException("Return reason cannot be empty");
        }
    }
    catch (const std::exception& e) {
        throw InventoryException("Error reading return operation: " + std::string(e.what()));
    }

    return is;
}
