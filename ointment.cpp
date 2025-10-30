#include "ointment.h"
#include <sstream>
#include <stdexcept>

Ointment::Ointment(std::string id, std::string name, double basePrice,
                   SafeDate expDate, std::string country,
                   bool prescription, std::string activeSubst, std::string instr,
                   double weight, std::string base)
    : Medicine(std::move(id), std::move(name), basePrice, expDate, std::move(country),
               prescription, std::move(activeSubst), std::move(instr))
    , weightG(weight)
    , baseType(std::move(base))
{
    if (weightG <= 0)
    {
        throw std::invalid_argument("Weight must be positive: " + std::to_string(weightG));
    }
    if (weightG > 1000)
    {
        throw std::invalid_argument("Weight too large: " + std::to_string(weightG));
    }
    if (baseType.empty())
    {
        throw std::invalid_argument("Base type cannot be empty");
    }
}

Ointment::Ointment()
    : Medicine()
    , weightG(0.0)
    , baseType("")
{
}

Ointment::Ointment(const Ointment& other)
    : Medicine(other)
    , weightG(other.weightG)
    , baseType(other.baseType)
{
}

std::string Ointment::getAdministrationMethod() const
{
    return "External";                             // Наружное применение
}

std::string Ointment::getDosageForm() const
{
    return "Ointment";                             // Лекарственная форма - мазь
}

Ointment& Ointment::operator=(const Ointment& other)
{
    if (this != &other)
    {
        Medicine::operator=(other);                // Присваиваем базовую часть

        weightG = other.weightG;                   // Присваиваем собственные поля
        baseType = other.baseType;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Ointment& ointment)
{
    os << static_cast<const Medicine&>(ointment); // Выводим базовую информацию

    os << "Weight: " << ointment.weightG << " g\n"
       << "Base type: " << ointment.baseType << "\n"
       << "Dosage Form: " << ointment.getDosageForm() << "\n"
       << "Administration: " << ointment.getAdministrationMethod() << "\n";

    return os;
}

std::istream& operator>>(std::istream& is, Ointment& ointment)
{
    try
    {
        is >> static_cast<Medicine&>(ointment);   // Читаем базовую информацию

        std::string temp;

        std::getline(is, temp);                   // "Weight: "
        std::getline(is, temp);                   // значение + " g"
        size_t pos = temp.find(" g");
        if (pos != std::string::npos)
        {
            temp = temp.substr(0, pos);           // Удаляем " g" из строки
        }
        ointment.weightG = std::stod(temp);       // Читаем вес

        std::getline(is, temp);                   // "Base type: "
        std::getline(is, ointment.baseType);      // Читаем тип основы

        std::getline(is, temp);                   // "Dosage Form: "
        std::getline(is, temp);                   // значение (пропускаем)
        std::getline(is, temp);                   // "Administration: "
        std::getline(is, temp);                   // значение (пропускаем)

    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading Ointment data: " + std::string(e.what()));
    }

    return is;
}
