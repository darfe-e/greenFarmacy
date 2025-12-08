#include "tablet.h"
#include "Exception/safeinput.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include <stdexcept>
#include <QDebug>

Tablet::Tablet(std::string id, std::string name, double basePrice,
               SafeDate expDate, std::string country,
               bool prescription, std::string activeSubst, std::string instr,
               int units, double dosage, std::string coating)
    : Medicine(std::move(id), std::move(name), basePrice, expDate, std::move(country),
               prescription, std::move(activeSubst), std::move(instr))
    , unitsPerPackage(units)
    , dosageMg(dosage)
    , coating(coating)
{
    qDebug() << "Создание Tablet:" << QString::fromStdString(id)              // Отладочный вывод информации
             << "coating:" << QString::fromStdString(coating);

    SafeInput::validateTextField(coating, "Coating");                         // Валидация поля "Покрытие"

    if (unitsPerPackage <= 0)                                                 // Проверка положительности количества таблеток
        throw InvalidProductDataException("Units per package", "must be positive");

    if (dosageMg <= 0)                                                        // Проверка положительности дозировки
        throw InvalidProductDataException("Dosage", "must be positive");

    if (coating.empty())                                                      // Проверка непустого покрытия
    {
        qDebug() << "ОШИБКА: coating пустой! ID:" << QString::fromStdString(id);  // Отладочное сообщение об ошибке
        throw InvalidProductDataException("Coating", "cannot be empty");
    }
}

Tablet::Tablet(const Tablet& other)
    : Medicine(other)                                                         // Копирование базовой части Medicine
    , unitsPerPackage(other.unitsPerPackage)                                  // Копирование количества таблеток
    , dosageMg(other.dosageMg)                                                // Копирование дозировки
    , coating(other.coating)                                                  // Копирование покрытия
{
}

std::string Tablet::getAdministrationMethod() const
{
    return "Oral";                                                            // Таблетки принимаются перорально (внутрь)
}

std::string Tablet::getDosageForm() const
{
    return "Tablet";                                                          // Форма выпуска - таблетки
}

Tablet& Tablet::operator=(const Tablet& other)
{
    if (this != &other)                                                       // Проверка самоприсваивания
    {
        Medicine::operator=(other);                                           // Присваивание базовой части Medicine
        unitsPerPackage = other.unitsPerPackage;                              // Копирование количества таблеток
        dosageMg = other.dosageMg;                                            // Копирование дозировки
        coating = other.coating;                                              // Копирование покрытия
    }
    return *this;                                                             // Возврат текущего объекта
}

std::ostream& operator<<(std::ostream& os, const Tablet& tablet)
{
    os << "[TABLET];";                                                        // Маркер типа для сериализации
    os << static_cast<const Medicine&>(tablet) << ";"                         // Вывод базовой информации Medicine
       << tablet.unitsPerPackage << ";"                                       // Вывод количества таблеток в упаковке
       << tablet.dosageMg << " mg" << ";"                                     // Вывод дозировки с единицей измерения
       << tablet.coating;                                                     // Вывод покрытия
    return os;                                                                // Возврат потока
}

std::istream& operator>>(std::istream& is, Tablet& tablet)
{
    try
    {
        std::vector<std::string> tokens;                                      // Вектор для хранения токенов
        std::string token;                                                    // Текущий токен

        for (int i = 0; i < 13; ++i)                                          // Чтение 13 полей для Tablet
        {
            if (!std::getline(is, token, ';'))                                // Чтение до разделителя ';'
                throw InvalidProductDataException("tablet data", "not enough fields");
            tokens.push_back(token);                                          // Сохранение токена
        }

        if (tokens.size() < 13)                                               // Проверка количества полей
            throw InvalidProductDataException("tablet data", "invalid number of fields");

        // Восстановление информации Medicine из токенов
        std::istringstream medicineStream(
            tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" +
            tokens[3] + ";" + tokens[4] + ";" + tokens[5] + ";" +
            tokens[6] + ";" + tokens[7] + ";" + tokens[8] + ";" + tokens[9]
            );
        medicineStream >> static_cast<Medicine&>(tablet);                     // Десериализация базовой части

        // Обработка количества таблеток (10-й токен)
        std::istringstream unitsStream(tokens[10]);
        tablet.unitsPerPackage = SafeInput::readPositiveInt(unitsStream, "Units per package");  // Чтение положительного целого

        // Обработка дозировки (11-й токен)
        std::string dosageStr = tokens[11];
        size_t pos = dosageStr.find(" mg");                                   // Поиск единицы измерения
        if (pos != std::string::npos)
            dosageStr = dosageStr.substr(0, pos);                             // Удаление " mg" из строки

        tablet.dosageMg = std::stod(dosageStr);                               // Преобразование строки в double
        if (tablet.dosageMg <= 0)                                             // Проверка положительности дозировки
            throw InvalidProductDataException("Dosage", "must be positive");

        // Обработка покрытия (12-й токен)
        tablet.coating = tokens[12];
        if (tablet.coating.empty())                                           // Проверка непустого покрытия
            throw InvalidProductDataException("Coating", "cannot be empty");
    }
    catch (const std::invalid_argument&)                                      // Ошибка преобразования числа
    {
        throw InvalidProductDataException("dosage", "must be a valid number");
    }
    catch (const PharmacyException&)                                          // Переброс исключений PharmacyException
    {
        throw;
    }
    catch (const std::exception& e)                                           // Обработка других исключений
    {
        throw InvalidProductDataException("tablet data", e.what());
    }

    return is;                                                                // Возврат потока
}
