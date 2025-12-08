#include "writeoff.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/InventoryExceptions/NegativeQuantityException.h"
#include "Exception/safeinput.h"
#include <sstream>
#include <stdexcept>

WriteOff::WriteOff(std::string id, SafeDate date,
                   std::shared_ptr<MedicalProduct> product, int quantity,
                   std::string reason, std::string status)
    : InventoryOperation(std::move(id), date, std::move(product), quantity, std::move(status))
    , writeOffReason(std::move(reason))
{
    if (writeOffReason.empty())                                                  // Проверка непустой причины списания
        throw InventoryException("Write-off reason cannot be empty");
}

WriteOff::WriteOff()
    : InventoryOperation()
    , writeOffReason("")
{
}

WriteOff::WriteOff(const WriteOff& other)
    : InventoryOperation(other)                                                  // Копирование базовой части операции
    , writeOffReason(other.writeOffReason)                                       // Копирование причины списания
{
}

std::string WriteOff::getOperationType() const
{
    return "WriteOff";                                                           // Возврат типа операции - "Списание"
}

void WriteOff::process()
{
    if (getStatus() == "completed")                                              // Проверка: операция уже выполнена
        throw InventoryException("Write-off operation already completed");

    if (getStatus() == "cancelled")                                              // Проверка: операция отменена
        throw InventoryException("Write-off operation is cancelled");

    if (getQuantity() <= 0)                                                      // Проверка положительности количества
        throw NegativeQuantityException(getQuantity());

    setStatus("completed");                                                      // Установка статуса "выполнено"
}

WriteOff& WriteOff::operator=(const WriteOff& other)
{
    if (this != &other)                                                          // Проверка самоприсваивания
    {
        InventoryOperation::operator=(other);                                    // Присваивание базовой части
        writeOffReason = other.writeOffReason;                                   // Копирование причины списания
    }
    return *this;                                                                // Возврат текущего объекта
}

std::ostream& operator<<(std::ostream& os, const WriteOff& writeOff)
{
    os << writeOff.id << ";"                                                     // Вывод ID операции
       << writeOff.operationDate.toString() << ";"                               // Вывод даты операции
       << writeOff.product->getId() << ";"                                       // Вывод ID продукта
       << writeOff.quantity << ";"                                               // Вывод количества
       << writeOff.status << ";"                                                 // Вывод статуса
       << writeOff.writeOffReason;                                               // Вывод причины списания
    return os;                                                                   // Возврат потока
}

std::istream& operator>>(std::istream& is, WriteOff& writeOff)
{
    std::string line;
    if (!std::getline(is, line) || line.empty())                                 // Чтение строки из потока
    {
        is.setstate(std::ios::failbit);                                          // Установка флага ошибки
        return is;                                                               // Возврат потока
    }

    std::vector<std::string> tokens;                                             // Вектор для хранения токенов
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, ';'))                                         // Разделение строки по ';'
        tokens.push_back(token);                                                 // Сохранение токена

    if (tokens.size() < 6)                                                       // Проверка количества полей
        throw InventoryException("Invalid number of fields for write-off operation");

    try
    {
        writeOff.id = tokens[0];                                                 // Чтение ID операции
        writeOff.operationDate = SafeDate::fromString(tokens[1]);                // Чтение и парсинг даты

        // Создание временного продукта (для сохранения структуры данных)
        writeOff.product = std::make_shared<MedicalProduct>(
            tokens[2], "Temp Product", 0.0, SafeDate(), "Unknown");

        writeOff.quantity = std::stoi(tokens[3]);                                // Чтение количества
        writeOff.status = tokens[4];                                             // Чтение статуса
        writeOff.writeOffReason = tokens[5];                                     // Чтение причины списания

        if (writeOff.writeOffReason.empty())                                     // Проверка непустой причины списания
            throw InventoryException("Write-off reason cannot be empty");
    }
    catch (const std::exception& e)                                              // Обработка исключений при чтении
    {
        throw InventoryException("Error reading write-off operation: " + std::string(e.what()));
    }

    return is;                                                                   // Возврат потока
}
