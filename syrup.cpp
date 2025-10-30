#include "syrup.h"
#include <sstream>
#include <stdexcept>

Syrup::Syrup(std::string id, std::string name, double basePrice,
             SafeDate expDate, std::string country,
             bool prescription, std::string activeSubst, std::string instr,
             double volume, bool sugar, std::string flavor)
    : Medicine(std::move(id), std::move(name), basePrice, expDate, std::move(country),
               prescription, std::move(activeSubst), std::move(instr))
    , volumeMl(volume)
    , hasSugar(sugar)
    , flavor(std::move(flavor))
{
    if (volumeMl <= 0)
        throw std::invalid_argument("Volume must be positive: " + std::to_string(volumeMl));
    if (volumeMl > 1000)
        throw std::invalid_argument("Volume too large: " + std::to_string(volumeMl));
    if (flavor.empty())
        throw std::invalid_argument("Flavor cannot be empty");
}

Syrup::Syrup()
    : Medicine()
    , volumeMl(0.0)
    , hasSugar(false)
    , flavor("")
{
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
    return "Oral";                                 // Пероральный прием
}

std::string Syrup::getDosageForm() const
{
    return "Syrup";                                // Лекарственная форма - сироп
}

Syrup& Syrup::operator=(const Syrup& other)
{
    if (this != &other)
    {
        Medicine::operator=(other);                // Присваиваем базовую часть

        volumeMl = other.volumeMl;                 // Присваиваем собственные поля
        hasSugar = other.hasSugar;
        flavor = other.flavor;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Syrup& syrup)
{
    os << static_cast<const Medicine&>(syrup);     // Выводим базовую информацию

    os << "Volume: " << syrup.volumeMl << " ml\n"  // Выводим специфичную информацию
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
        is >> static_cast<Medicine&>(syrup);       // Читаем базовую информацию

        std::string temp;

        std::getline(is, temp);                    // "Volume: "
        std::getline(is, temp);                    // значение + " ml"
        size_t pos = temp.find(" ml");
        if (pos != std::string::npos)
        {
            temp = temp.substr(0, pos);            // Удаляем " ml" из строки
        }
        syrup.volumeMl = std::stod(temp);

        std::getline(is, temp);                    // "Contains Sugar: "
        std::getline(is, temp);
        syrup.hasSugar = (temp == "Yes");          // Преобразуем в bool

        std::getline(is, temp);                    // "Flavor: "
        std::getline(is, syrup.flavor);            // Читаем вкус

        std::getline(is, temp);                    // "Dosage Form: "
        std::getline(is, temp);                    // значение (пропускаем)
        std::getline(is, temp);                    // "Administration: "
        std::getline(is, temp);                    // значение (пропускаем)

    }
    catch (const std::exception& e)
    {
        is.setstate(std::ios::failbit);
        throw std::runtime_error("Error reading Syrup data: " + std::string(e.what()));
    }

    return is;
}
