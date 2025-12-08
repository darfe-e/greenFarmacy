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
    if (this->reason.empty())                                                       // Проверка непустой причины возврата
        throw InventoryException("Return reason cannot be empty");
}

Return::Return()
    : InventoryOperation()
    , reason("")
{
}

Return::Return(const Return& other)
    : InventoryOperation(other)                                                     // Копирование базовой части операции
    , reason(other.reason)                                                          // Копирование причины возврата
{
}

std::string Return::getOperationType() const
{
    return "Return";                                                                // Возврат типа операции - "Возврат"
}

void Return::process()
{
    if (getStatus() == "completed")                                                 // Проверка: операция уже выполнена
        throw InventoryException("Return operation already completed");

    if (getStatus() == "cancelled")                                                 // Проверка: операция отменена
        throw InventoryException("Return operation is cancelled");

    setStatus("completed");                                                         // Установка статуса "выполнено"
}

Return& Return::operator=(const Return& other)
{
    if (this != &other)                                                             // Проверка самоприсваивания
    {
        InventoryOperation::operator=(other);                                       // Присваивание базовой части
        reason = other.reason;                                                      // Копирование причины возврата
    }
    return *this;                                                                   // Возврат текущего объекта
}

std::ostream& operator<<(std::ostream& os, const Return& returnOp)
{
    os << returnOp.id << ";"                                                        // Вывод ID операции
       << returnOp.operationDate.toString() << ";"                                  // Вывод даты операции
       << returnOp.product->getId() << ";"                                          // Вывод ID продукта
       << returnOp.quantity << ";"                                                  // Вывод количества
       << returnOp.status << ";"                                                    // Вывод статуса
       << returnOp.reason;                                                          // Вывод причины возврата
    return os;                                                                      // Возврат потока
}

std::istream& operator>>(std::istream& is, Return& returnOp)
{
    std::string line;
    if (!std::getline(is, line) || line.empty())                                    // Чтение строки из потока
    {
        is.setstate(std::ios::failbit);                                             // Установка флага ошибки
        return is;                                                                  // Возврат потока
    }

    std::vector<std::string> tokens;                                                // Вектор для хранения токенов
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, ';'))                                            // Разделение строки по ';'
        tokens.push_back(token);                                                    // Сохранение токена

    if (tokens.size() < 6)                                                          // Проверка количества полей
        throw InventoryException("Invalid number of fields for return operation");

    try
    {
        returnOp.id = tokens[0];                                                    // Чтение ID операции
        returnOp.operationDate = SafeDate::fromString(tokens[1]);                   // Чтение и парсинг даты

        // Создание временного продукта (для сохранения структуры данных)
        returnOp.product = std::make_shared<MedicalProduct>(
            tokens[2], "Temp Product", 0.0, SafeDate(), "Unknown");

        returnOp.quantity = std::stoi(tokens[3]);                                   // Чтение количества
        returnOp.status = tokens[4];                                                // Чтение статуса
        returnOp.reason = tokens[5];                                                // Чтение причины возврата

        if (returnOp.reason.empty())                                                // Проверка непустой причины
            throw InventoryException("Return reason cannot be empty");
    }
    catch (const std::exception& e)                                                 // Обработка исключений при чтении
    {
        throw InventoryException("Error reading return operation: " + std::string(e.what()));
    }

    return is;                                                                      // Возврат потока
}
