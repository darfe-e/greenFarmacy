#include "ointment.h"
#include <sstream>
#include <stdexcept>
#include "Exception/safeinput.h"
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"

// ... остальной код ...

Ointment::Ointment(std::string id, std::string name, double basePrice,
                   SafeDate expDate, std::string country,
                   bool prescription, std::string activeSubst, std::string instr,
                   double weight, std::string base)
    : Medicine(std::move(id), std::move(name), basePrice, expDate, std::move(country),
               prescription, std::move(activeSubst), std::move(instr))
    , weightG(weight)
    , baseType(base)
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
        is >> static_cast<Medicine&>(ointment);

        // Weight
        SafeInput::skipLabel(is); // "Weight: "
        std::string weightStr = SafeInput::readNonEmptyString(is, "Weight");

        // Удаляем " g" из строки
        size_t pos = weightStr.find(" g");
        if (pos != std::string::npos)
        {
            weightStr = weightStr.substr(0, pos);
        }

        ointment.weightG = std::stod(weightStr);
        if (ointment.weightG <= 0)
        {
            throw InvalidProductDataException("weight", "must be positive");
        }

        // Base type
        SafeInput::skipLabel(is); // "Base type: "
        ointment.baseType = SafeInput::readNonEmptyString(is, "Base type");

        // Пропускаем остальные поля
        SafeInput::skipLabel(is); // "Dosage Form: "
        SafeInput::skipLabel(is); // значение
        SafeInput::skipLabel(is); // "Administration: "
        SafeInput::skipLabel(is); // значение

    }
    catch (const std::invalid_argument&)
    {
        throw InvalidProductDataException("weight", "must be a valid number");
    }
    catch (const PharmacyException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("ointment data", e.what());
    }

    return is;
}
