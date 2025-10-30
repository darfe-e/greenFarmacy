#include "tablet.h"
#include <sstream>
#include <stdexcept>

Tablet::Tablet(std::string id, std::string name, double basePrice,
               SafeDate expDate, std::string country,
               bool prescription, std::string activeSubst, std::string instr,
               int units, double dosage, std::string coating)
    : Medicine(std::move(id), std::move(name), basePrice, expDate, std::move(country),
               prescription, std::move(activeSubst), std::move(instr))
    , unitsPerPackage(units)
    , dosageMg(dosage)
    , coating(std::move(coating))
{
    if (unitsPerPackage <= 0)
    {
        throw std::invalid_argument("Units per package must be positive: " + std::to_string(unitsPerPackage));
    }
    if (dosageMg <= 0)
    {
        throw std::invalid_argument("Dosage must be positive: " + std::to_string(dosageMg));
    }
    if (coating.empty())
    {
        throw std::invalid_argument("Coating cannot be empty");
    }
}

Tablet::Tablet()
    : Medicine()
    , unitsPerPackage(0)
    , dosageMg(0.0)
    , coating("")
{
}

Tablet::Tablet(const Tablet& other)
    : Medicine(other)
    , unitsPerPackage(other.unitsPerPackage)
    , dosageMg(other.dosageMg)
    , coating(other.coating)
{
}

std::string Tablet::getAdministrationMethod() const
{
    return "Oral";                                 // Пероральный прием
}

std::string Tablet::getDosageForm() const
{
    return "Tablet";                               // Лекарственная форма - таблетка
}

Tablet& Tablet::operator=(const Tablet& other)
{
    if (this != &other)
    {
        Medicine::operator=(other);                // Присваиваем базовую часть

        unitsPerPackage = other.unitsPerPackage;   // Присваиваем собственные поля
        dosageMg = other.dosageMg;
        coating = other.coating;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Tablet& tablet)
{
    os << static_cast<const Medicine&>(tablet);    // Выводим базовую информацию

    os << "Units per package: " << tablet.unitsPerPackage << "\n"
       << "Dosage: " << tablet.dosageMg << " mg\n"
       << "Coating: " << tablet.coating << "\n"
       << "Dosage Form: " << tablet.getDosageForm() << "\n"
       << "Administration: " << tablet.getAdministrationMethod() << "\n";

    return os;
}

std::istream& operator>>(std::istream& is, Tablet& tablet)
{
    try
    {
        is >> static_cast<Medicine&>(tablet);      // Читаем базовую информацию

        std::string temp;

        std::getline(is, temp);                    // "Units per package: "
        std::getline(is, temp);
        tablet.unitsPerPackage = std::stoi(temp);  // Читаем количество таблеток

        std::getline(is, temp);                    // "Dosage: "
        std::getline(is, temp);                    // значение + " mg"
        size_t pos = temp.find(" mg");
        if (pos != std::string::npos)
        {
            temp = temp.substr(0, pos);            // Удаляем " mg" из строки
        }
        tablet.dosageMg = std::stod(temp);         // Читаем дозировку

        std::getline(is, temp);                    // "Coating: "
        std::getline(is, tablet.coating);          // Читаем тип оболочки

        std::getline(is, temp);                    // "Dosage Form: "
        std::getline(is, temp);                    // значение (пропускаем)
        std::getline(is, temp);                    // "Administration: "
        std::getline(is, temp);                    // значение (пропускаем)

    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading Tablet data: " + std::string(e.what()));
    }

    return is;
}
