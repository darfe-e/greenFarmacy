#include "supply.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/InventoryExceptions/NegativeQuantityException.h"
#include "Exception/safeinput.h"
#include <sstream>
#include <stdexcept>

Supply::Supply(std::string id, SafeDate date,
               std::shared_ptr<MedicalProduct> product, int quantity,
               std::string src, std::string dest, std::string status)
    : InventoryOperation(std::move(id), date, std::move(product), quantity, std::move(status))
    , source(std::move(src))
    , destination(std::move(dest))
{
    if (source.empty())                                                           // Проверка непустого источника поставки
        throw InventoryException("Supply source cannot be empty");
    if (destination.empty())                                                      // Проверка непустого назначения поставки
        throw InventoryException("Supply destination cannot be empty");
}

Supply::Supply()
    : InventoryOperation()
    , source("")
    , destination("")
{
}

Supply::Supply(const Supply& other)
    : InventoryOperation(other)                                                   // Копирование базовой части операции
    , source(other.source)                                                        // Копирование источника
    , destination(other.destination)                                              // Копирование назначения
{
}

std::string Supply::getOperationType() const
{
    return "Supply";                                                              // Возврат типа операции - "Поставка"
}

void Supply::process()
{
    if (getStatus() == "completed")                                              // Проверка: операция уже выполнена
        throw InventoryException("Supply operation already completed");

    if (getStatus() == "cancelled")                                              // Проверка: операция отменена
        throw InventoryException("Supply operation is cancelled");

    if (getQuantity() <= 0)                                                      // Проверка положительности количества
        throw NegativeQuantityException(getQuantity());

    setStatus("completed");                                                       // Установка статуса "выполнено"
}

Supply& Supply::operator=(const Supply& other)
{
    if (this != &other)                                                          // Проверка самоприсваивания
    {
        InventoryOperation::operator=(other);                                    // Присваивание базовой части
        source = other.source;                                                   // Копирование источника
        destination = other.destination;                                         // Копирование назначения
    }
    return *this;                                                                // Возврат текущего объекта
}

std::ostream& operator<<(std::ostream& os, const Supply& supply)
{
    os << supply.id << ";"                                                        // Вывод ID операции
       << supply.operationDate.toString() << ";"                                  // Вывод даты операции
       << supply.product->getId() << ";"                                          // Вывод ID продукта
       << supply.quantity << ";"                                                  // Вывод количества
       << supply.status << ";"                                                    // Вывод статуса
       << supply.source << ";"                                                    // Вывод источника поставки
       << supply.destination;                                                     // Вывод назначения поставки
    return os;                                                                    // Возврат потока
}

std::istream& operator>>(std::istream& is, Supply& supply)
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

    if (tokens.size() < 7)                                                       // Проверка количества полей
        throw InventoryException("Invalid number of fields for supply operation");

    try
    {
        supply.id = tokens[0];                                                   // Чтение ID операции
        supply.operationDate = SafeDate::fromString(tokens[1]);                  // Чтение и парсинг даты

        // Создание временного продукта (для сохранения структуры данных)
        supply.product = std::make_shared<MedicalProduct>(
            tokens[2], "Temp Product", 0.0, SafeDate(), "Unknown");

        supply.quantity = std::stoi(tokens[3]);                                  // Чтение количества
        supply.status = tokens[4];                                               // Чтение статуса
        supply.source = tokens[5];                                               // Чтение источника поставки
        supply.destination = tokens[6];                                          // Чтение назначения поставки

        if (supply.source.empty() || supply.destination.empty())                 // Проверка непустых источника и назначения
            throw InventoryException("Supply source and destination cannot be empty");
    }
    catch (const std::exception& e)                                              // Обработка исключений при чтении
    {
        throw InventoryException("Error reading supply operation: " + std::string(e.what()));
    }

    return is;                                                                   // Возврат потока
}
