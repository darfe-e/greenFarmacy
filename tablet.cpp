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
    qDebug() << "Создание Tablet:" << QString::fromStdString(id)
             << "coating:" << QString::fromStdString(coating);

    if (unitsPerPackage <= 0)
    {
        throw InvalidProductDataException("Units per package", "must be positive");
    }
    if (dosageMg <= 0)
    {
        throw InvalidProductDataException("Dosage", "must be positive");
    }
    if (coating.empty())
    {
        qDebug() << "ОШИБКА: coating пустой! ID:" << QString::fromStdString(id);
        throw InvalidProductDataException("Coating", "cannot be empty");
    }
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
    return "Oral";
}

std::string Tablet::getDosageForm() const
{
    return "Tablet";
}

Tablet& Tablet::operator=(const Tablet& other)
{
    if (this != &other)
    {
        Medicine::operator=(other);
        unitsPerPackage = other.unitsPerPackage;
        dosageMg = other.dosageMg;
        coating = other.coating;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Tablet& tablet)
{
    os << static_cast<const Medicine&>(tablet);

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
        is >> static_cast<Medicine&>(tablet);

        // Units per package
        SafeInput::skipLabel(is); // "Units per package: "
        tablet.unitsPerPackage = SafeInput::readPositiveInt(is, "Units per package");

        // Dosage
        SafeInput::skipLabel(is); // "Dosage: "
        std::string dosageStr = SafeInput::readNonEmptyString(is, "Dosage");

        // Удаляем " mg" из строки
        size_t pos = dosageStr.find(" mg");
        if (pos != std::string::npos)
        {
            dosageStr = dosageStr.substr(0, pos);
        }

        tablet.dosageMg = std::stod(dosageStr);
        if (tablet.dosageMg <= 0)
        {
            throw InvalidProductDataException("Dosage", "must be positive");
        }

        // Coating
        SafeInput::skipLabel(is); // "Coating: "
        tablet.coating = SafeInput::readNonEmptyString(is, "Coating");

        // Пропускаем остальные поля
        SafeInput::skipLabel(is); // "Dosage Form: "
        SafeInput::skipLabel(is); // значение
        SafeInput::skipLabel(is); // "Administration: "
        SafeInput::skipLabel(is); // значение

    }
    catch (const std::invalid_argument&)
    {
        throw InvalidProductDataException("dosage", "must be a valid number");
    }
    catch (const PharmacyException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("tablet data", e.what());
    }

    return is;
}
