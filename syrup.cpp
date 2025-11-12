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
    if (volumeMl <= 0)
    {
        throw InvalidProductDataException("Volume", "must be positive");
    }
    if (volumeMl > 1000)
    {
        throw InvalidProductDataException("Volume", "too large");
    }
    if (flavor.empty())
    {
        throw InvalidProductDataException("Flavor", "cannot be empty");
    }
}

Syrup::Syrup(const Syrup& other)
    : Medicine(other)
    , volumeMl(other.volumeMl)
    , hasSugar(other.hasSugar)
    , flavor(other.flavor)
{
}

std::string Syrup::getAdministrationMethod() const
{
    return "Oral";
}

std::string Syrup::getDosageForm() const
{
    return "Syrup";
}

Syrup& Syrup::operator=(const Syrup& other)
{
    if (this != &other)
    {
        Medicine::operator=(other);
        volumeMl = other.volumeMl;
        hasSugar = other.hasSugar;
        flavor = other.flavor;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Syrup& syrup)
{
    os << static_cast<const Medicine&>(syrup);

    os << "Volume: " << syrup.volumeMl << " ml\n"
       << "Contains Sugar: " << (syrup.hasSugar ? "Yes" : "No") << "\n"
       << "Flavor: " << syrup.flavor << "\n"
       << "Dosage Form: " << syrup.getDosageForm() << "\n"
       << "Administration: " << syrup.getAdministrationMethod() << "\n";

    return os;
}

std::istream& operator>>(std::istream& is, Syrup& syrup)
{
    try
    {
        is >> static_cast<Medicine&>(syrup);

        // Volume
        SafeInput::skipLabel(is); // "Volume: "
        std::string volumeStr = SafeInput::readNonEmptyString(is, "Volume");

        // Удаляем " ml" из строки
        size_t pos = volumeStr.find(" ml");
        if (pos != std::string::npos)
        {
            volumeStr = volumeStr.substr(0, pos);
        }

        syrup.volumeMl = std::stod(volumeStr);
        if (syrup.volumeMl <= 0)
        {
            throw InvalidProductDataException("Volume", "must be positive");
        }

        // Contains Sugar
        SafeInput::skipLabel(is); // "Contains Sugar: "
        syrup.hasSugar = SafeInput::readBoolean(is, "Contains sugar");

        // Flavor
        SafeInput::skipLabel(is); // "Flavor: "
        syrup.flavor = SafeInput::readNonEmptyString(is, "Flavor");

        // Пропускаем остальные поля
        SafeInput::skipLabel(is); // "Dosage Form: "
        SafeInput::skipLabel(is); // значение
        SafeInput::skipLabel(is); // "Administration: "
        SafeInput::skipLabel(is); // значение

    }
    catch (const std::invalid_argument&)
    {
        throw InvalidProductDataException("volume", "must be a valid number");
    }
    catch (const PharmacyException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("syrup data", e.what());
    }

    return is;
}
