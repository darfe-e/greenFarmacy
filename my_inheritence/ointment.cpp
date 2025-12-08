#include "ointment.h"
#include <sstream>
#include <stdexcept>
#include "Exception/safeinput.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"

// Конструктор с параметрами
Ointment::Ointment(std::string id, std::string name, double basePrice,
                   SafeDate expDate, std::string country,
                   bool prescription, std::string activeSubst, std::string instr,
                   double weight, std::string base)
    : Medicine(std::move(id), std::move(name), basePrice, expDate, std::move(country),
               prescription, std::move(activeSubst), std::move(instr))
    , weightG(weight)
    , baseType(base)
{
    SafeInput::validateTextField(baseType, "Base type");     // Валидация типа основы

    if (weightG <= 0)                                        // Проверка положительности веса
        throw std::invalid_argument("Weight must be positive: " + std::to_string(weightG));

    if (weightG > 1000)                                      // Проверка максимального веса
        throw std::invalid_argument("Weight too large: " + std::to_string(weightG));

    if (baseType.empty())                                    // Проверка непустого типа основы
        throw std::invalid_argument("Base type cannot be empty");
}

// Конструктор копирования
Ointment::Ointment(const Ointment& other)
    : Medicine(other)                                        // Копирование базовой части Medicine
    , weightG(other.weightG)                                 // Копирование веса
    , baseType(other.baseType)                               // Копирование типа основы
{
}

// Получение способа применения
std::string Ointment::getAdministrationMethod() const
{
    return "External";                                       // Мазь применяется наружно
}

// Получение лекарственной формы
std::string Ointment::getDosageForm() const
{
    return "Ointment";                                       // Форма выпуска - мазь
}

// Оператор присваивания
Ointment& Ointment::operator=(const Ointment& other)
{
    if (this != &other)                                      // Проверка самоприсваивания
    {
        Medicine::operator=(other);                          // Присваивание базовой части Medicine
        weightG = other.weightG;                             // Копирование веса
        baseType = other.baseType;                           // Копирование типа основы
    }
    return *this;                                            // Возврат текущего объекта
}

// Оператор вывода в поток
std::ostream& operator<<(std::ostream& os, const Ointment& ointment)
{
    os << "[OINTMENT];";                                     // Маркер типа для сериализации
    os << static_cast<const Medicine&>(ointment) << ";"      // Вывод базовой информации Medicine
       << ointment.weightG << " g" << ";"                    // Вывод веса с единицей измерения
       << ointment.baseType;                                 // Вывод типа основы
    return os;                                               // Возврат потока
}

// Оператор ввода из потока
std::istream& operator>>(std::istream& is, Ointment& ointment)
{
    try
    {
        std::vector<std::string> tokens;                     // Вектор для хранения токенов
        std::string token;                                   // Текущий токен

        for (int i = 0; i < 12; ++i)                         // Чтение 12 полей для Ointment
        {
            if (!std::getline(is, token, ';'))               // Чтение до разделителя ';'
                throw InvalidProductDataException("ointment data", "not enough fields");
            tokens.push_back(token);                         // Сохранение токена
        }

        if (tokens.size() < 12)                              // Проверка количества полей
            throw InvalidProductDataException("ointment data", "invalid number of fields");

        // Восстановление информации Medicine из токенов
        std::istringstream medicineStream(
            tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" +
            tokens[3] + ";" + tokens[4] + ";" + tokens[5] + ";" +
            tokens[6] + ";" + tokens[7] + ";" + tokens[8] + ";" + tokens[9]
            );
        medicineStream >> static_cast<Medicine&>(ointment);  // Десериализация базовой части

        // Обработка веса (10-й токен)
        std::string weightStr = tokens[10];
        size_t pos = weightStr.find(" g");                   // Поиск единицы измерения
        if (pos != std::string::npos)
            weightStr = weightStr.substr(0, pos);            // Удаление " g" из строки

        ointment.weightG = std::stod(weightStr);             // Преобразование строки в double
        if (ointment.weightG <= 0)                           // Проверка положительности веса
            throw InvalidProductDataException("weight", "must be positive");

        // Обработка типа основы (11-й токен)
        ointment.baseType = tokens[11];
        if (ointment.baseType.empty())                       // Проверка непустого типа основы
            throw InvalidProductDataException("Base type", "cannot be empty");
    }
    catch (const std::invalid_argument&)                     // Ошибка преобразования числа
    {
        throw InvalidProductDataException("weight", "must be a valid number");
    }
    catch (const PharmacyException&)                         // Переброс исключений PharmacyException
    {
        throw;
    }
    catch (const std::exception& e)                          // Обработка других исключений
    {
        throw InvalidProductDataException("ointment data", e.what());
    }

    return is;                                               // Возврат потока
}
