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

     SafeInput::validateTextField(flavor, "Flavor");

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
    os << "[SYRUP];"; // Маркер типа
    os << static_cast<const Medicine&>(syrup) << ";"
       << syrup.volumeMl << " ml" << ";"
       << (syrup.hasSugar ? "Yes" : "No") << ";"
       << syrup.flavor;
    return os;
}

std::istream& operator>>(std::istream& is, Syrup& syrup)
{
    try
    {
        std::vector<std::string> tokens;
        std::string token;

        for (int i = 0; i < 13; ++i) { // 13 полей для Syrup
            if (!std::getline(is, token, ';')) {
                throw InvalidProductDataException("syrup data", "not enough fields");
            }
            tokens.push_back(token);
        }

        if (tokens.size() < 13) {
            throw InvalidProductDataException("syrup data", "invalid number of fields");
        }

        // Восстанавливаем базовую информацию Medicine
        std::istringstream medicineStream(
            tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" +
            tokens[3] + ";" + tokens[4] + ";" + tokens[5] + ";" +
            tokens[6] + ";" + tokens[7] + ";" + tokens[8] + ";" + tokens[9]
            );
        medicineStream >> static_cast<Medicine&>(syrup);

        // Volume (10-е поле в tokens)
        std::string volumeStr = tokens[10];
        size_t pos = volumeStr.find(" ml");
        if (pos != std::string::npos) {
            volumeStr = volumeStr.substr(0, pos);
        }

        syrup.volumeMl = std::stod(volumeStr);
        if (syrup.volumeMl <= 0) {
            throw InvalidProductDataException("Volume", "must be positive");
        }

        // Contains Sugar (11-е поле в tokens)
        std::istringstream sugarStream(tokens[11]);
        syrup.hasSugar = SafeInput::readBoolean(sugarStream, "Contains sugar");

        // Flavor (12-е поле в tokens)
        syrup.flavor = tokens[12];
        if (syrup.flavor.empty()) {
            throw InvalidProductDataException("Flavor", "cannot be empty");
        }

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
