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
    os << "[TABLET];"; // Маркер типа
    os << static_cast<const Medicine&>(tablet) << ";"
       << tablet.unitsPerPackage << ";"
       << tablet.dosageMg << " mg" << ";"
       << tablet.coating;
    return os;
}

std::istream& operator>>(std::istream& is, Tablet& tablet)
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

        if (tokens.size() < 15) {
            throw InvalidProductDataException("tablet data", "invalid number of fields");
        }

        // Восстанавливаем базовую информацию Medicine
        std::istringstream medicineStream(tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" + tokens[3] + ";" + tokens[4] + ";" + tokens[5] + ";" + tokens[6] + ";" + tokens[7] + ";" + tokens[8] + ";" + tokens[9]);
        medicineStream >> static_cast<Medicine&>(tablet);

        // Units per package (11-е поле)
        std::istringstream unitsStream(tokens[10]);
        tablet.unitsPerPackage = SafeInput::readPositiveInt(unitsStream, "Units per package");

        // Dosage (12-е поле)
        std::string dosageStr = tokens[11];
        size_t pos = dosageStr.find(" mg");
        if (pos != std::string::npos) {
            dosageStr = dosageStr.substr(0, pos);
        }

        tablet.dosageMg = std::stod(dosageStr);
        if (tablet.dosageMg <= 0) {
            throw InvalidProductDataException("Dosage", "must be positive");
        }

        // Coating (13-е поле)
        tablet.coating = tokens[12];
        if (tablet.coating.empty()) {
            throw InvalidProductDataException("Coating", "cannot be empty");
        }

        // Dosage Form и Administration Method (14-е и 15-е поля) - только для вывода

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
