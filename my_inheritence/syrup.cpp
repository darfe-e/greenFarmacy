#include "syrup.h"
#include "Exception/safeinput.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include <stdexcept>

Syrup::Syrup(std::string id, std::string name, double basePrice,
             SafeDate expDate, std::string country,
             bool prescription, std::string activeSubst, std::string instr,
             double volume, bool sugar, std::string flavor)
    : Medicine(std::move(id), std::move(name), basePrice, expDate, std::move(country),
               prescription, std::move(activeSubst), std::move(instr))
    , volumeMl(volume)
    , hasSugar(sugar)
    , flavor(flavor)
{
    SafeInput::validateTextField(flavor, "Flavor");                              // Валидация поля "Вкус"

    if (volumeMl <= 0)                                                           // Проверка положительности объема
        throw InvalidProductDataException("Volume", "must be positive");

    if (volumeMl > 1000)                                                         // Проверка максимального объема
        throw InvalidProductDataException("Volume", "too large");

    if (flavor.empty())                                                          // Проверка непустого вкуса
        throw InvalidProductDataException("Flavor", "cannot be empty");
}

Syrup::Syrup(const Syrup& other)
    : Medicine(other)                                                            // Копирование базовой части Medicine
    , volumeMl(other.volumeMl)                                                   // Копирование объема
    , hasSugar(other.hasSugar)                                                   // Копирование флага содержания сахара
    , flavor(other.flavor)                                                       // Копирование вкуса
{
}

std::string Syrup::getAdministrationMethod() const
{
    return "Oral";                                                               // Сироп принимается перорально (внутрь)
}

std::string Syrup::getDosageForm() const
{
    return "Syrup";                                                              // Форма выпуска - сироп
}

Syrup& Syrup::operator=(const Syrup& other)
{
    if (this != &other)                                                          // Проверка самоприсваивания
    {
        Medicine::operator=(other);                                              // Присваивание базовой части Medicine
        volumeMl = other.volumeMl;                                               // Копирование объема
        hasSugar = other.hasSugar;                                               // Копирование флага содержания сахара
        flavor = other.flavor;                                                   // Копирование вкуса
    }
    return *this;                                                                // Возврат текущего объекта
}

std::ostream& operator<<(std::ostream& os, const Syrup& syrup)
{
    os << "[SYRUP];";                                                            // Маркер типа для сериализации
    os << static_cast<const Medicine&>(syrup) << ";"                             // Вывод базовой информации Medicine
       << syrup.volumeMl << " ml" << ";"                                         // Вывод объема с единицей измерения
       << (syrup.hasSugar ? "Yes" : "No") << ";"                                 // Вывод наличия сахара
       << syrup.flavor;                                                          // Вывод вкуса
    return os;                                                                   // Возврат потока
}

std::istream& operator>>(std::istream& is, Syrup& syrup)
{
    try
    {
        std::vector<std::string> tokens;                                         // Вектор для хранения токенов
        std::string token;                                                       // Текущий токен

        for (int i = 0; i < 13; ++i)                                             // Чтение 13 полей для Syrup
        {
            if (!std::getline(is, token, ';'))                                   // Чтение до разделителя ';'
                throw InvalidProductDataException("syrup data", "not enough fields");
            tokens.push_back(token);                                             // Сохранение токена
        }

        if (tokens.size() < 13)                                                  // Проверка количества полей
            throw InvalidProductDataException("syrup data", "invalid number of fields");

        // Восстановление информации Medicine из токенов
        std::istringstream medicineStream(
            tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" +
            tokens[3] + ";" + tokens[4] + ";" + tokens[5] + ";" +
            tokens[6] + ";" + tokens[7] + ";" + tokens[8] + ";" + tokens[9]
            );
        medicineStream >> static_cast<Medicine&>(syrup);                         // Десериализация базовой части

        // Обработка объема (10-й токен)
        std::string volumeStr = tokens[10];
        size_t pos = volumeStr.find(" ml");                                      // Поиск единицы измерения
        if (pos != std::string::npos)
            volumeStr = volumeStr.substr(0, pos);                                // Удаление " ml" из строки

        syrup.volumeMl = std::stod(volumeStr);                                   // Преобразование строки в double
        if (syrup.volumeMl <= 0)                                                 // Проверка положительности объема
            throw InvalidProductDataException("Volume", "must be positive");

        // Обработка наличия сахара (11-й токен)
        std::istringstream sugarStream(tokens[11]);
        syrup.hasSugar = SafeInput::readBoolean(sugarStream, "Contains sugar");  // Чтение булевого значения

        // Обработка вкуса (12-й токен)
        syrup.flavor = tokens[12];
        if (syrup.flavor.empty())                                                // Проверка непустого вкуса
            throw InvalidProductDataException("Flavor", "cannot be empty");
    }
    catch (const std::invalid_argument&)                                         // Ошибка преобразования числа
    {
        throw InvalidProductDataException("volume", "must be a valid number");
    }
    catch (const PharmacyException&)                                             // Переброс исключений PharmacyException
    {
        throw;
    }
    catch (const std::exception& e)                                              // Обработка других исключений
    {
        throw InvalidProductDataException("syrup data", e.what());
    }

    return is;                                                                   // Возврат потока
}
