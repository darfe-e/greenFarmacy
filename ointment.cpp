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
    os << "[OINTMENT];"; // Маркер типа
    os << static_cast<const Medicine&>(ointment) << ";"
       << ointment.weightG << " g" << ";"
       << ointment.baseType;
    return os;
}

std::istream& operator>>(std::istream& is, Ointment& ointment)
{
    try
    {
        std::string line;
        std::getline(is, line);

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ';')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 14) {
            throw InvalidProductDataException("ointment data", "invalid number of fields");
        }

        // Восстанавливаем базовую информацию Medicine
        std::istringstream medicineStream(tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" + tokens[3] + ";" + tokens[4] + ";" + tokens[5] + ";" + tokens[6] + ";" + tokens[7] + ";" + tokens[8] + ";" + tokens[9]);
        medicineStream >> static_cast<Medicine&>(ointment);

        // Weight (11-е поле)
        std::string weightStr = tokens[10];
        size_t pos = weightStr.find(" g");
        if (pos != std::string::npos) {
            weightStr = weightStr.substr(0, pos);
        }

        ointment.weightG = std::stod(weightStr);
        if (ointment.weightG <= 0) {
            throw InvalidProductDataException("weight", "must be positive");
        }

        // Base type (12-е поле)
        ointment.baseType = tokens[11];
        if (ointment.baseType.empty()) {
            throw InvalidProductDataException("Base type", "cannot be empty");
        }

        // Dosage Form и Administration Method (13-е и 14-е поля) - только для вывода

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
