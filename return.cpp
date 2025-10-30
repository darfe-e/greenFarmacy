#include "return.h"
#include <sstream>
#include <stdexcept>

Return::Return(std::string id, SafeDate date,
               std::shared_ptr<MedicalProduct> product, int quantity,
               std::string reason, std::string status)
    : InventoryOperation(std::move(id), date, std::move(product), quantity, std::move(status))
    , reason(std::move(reason))
{
    if (this->reason.empty())
        throw std::invalid_argument("Return reason cannot be empty");
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
    // Используем методы, которые должны быть в базовом классе InventoryOperation
    // Если этих методов нет, нужно либо добавить их в базовый класс,
    // либо использовать альтернативный подход

    // Вариант 1: Если в базовом классе есть методы getStatus()/setStatus()
    if (getStatus() == "completed")
        throw std::runtime_error("Return operation already completed");

    if (getStatus() == "cancelled")
        throw std::runtime_error("Return operation is cancelled");

    // Логика обработки возврата
    // В реальной системе здесь была бы бизнес-логика

    setStatus("completed"); // Помечаем как завершенную
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

        std::string temp;
        std::getline(is, temp); // Пропускаем "Reason: " если он есть в потоке

        // Читаем до конца строки или следующий токен
        if (temp.find("Reason:") != std::string::npos) {
            // Если в temp уже есть "Reason: ", то причина - остаток строки
            returnOp.reason = temp.substr(temp.find("Reason:") + 7);
        } else {
            // Иначе читаем следующую строку как причину
            std::getline(is, returnOp.reason);
        }

    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading Return data: " + std::string(e.what()));
    }

    return is;
}
