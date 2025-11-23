#include "medicine.h"
#include <algorithm>
#include <sstream>
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/safeinput.h"

// Конструктор с параметрами
Medicine::Medicine(std::string id, std::string name, double basePrice,
                   SafeDate expDate, std::string country,
                   bool prescription, std::string activeSubst, std::string instr)
    : MedicalProduct(std::move(id), std::move(name), basePrice, expDate, std::move(country)),
    isPrescription(prescription),
    activeSubstance(std::move(activeSubst)),
    instructions(std::move(instr))
{
    // Валидация параметров
    if (activeSubstance.empty())
        throw std::invalid_argument("Active substance cannot be empty");
    if (instructions.empty())
        throw std::invalid_argument("Instructions cannot be empty");
}

// Конструктор по умолчанию
Medicine::Medicine()
    : MedicalProduct(),
    isPrescription(false),
    activeSubstance(""),
    instructions("")
{
}

// Конструктор копирования
Medicine::Medicine(const Medicine& other)
    : MedicalProduct(other),
    isPrescription(other.isPrescription),
    activeSubstance(other.activeSubstance),
    instructions(other.instructions),
    analogues(other.analogues) // Копируем shared_ptr (поверхностное копирование)
{
}

// Добавление аналога
void Medicine::addAnalogue(std::shared_ptr<Medicine> analogue)
{
    if (!analogue)
        throw std::invalid_argument("Cannot add null analogue");

    // Проверяем, не является ли лекарство аналогом самого себя
    if (analogue->getId() == this->getId())
        throw std::invalid_argument("Medicine cannot be analogue of itself");

    // Проверяем, нет ли уже такого аналога
    auto it = std::find_if(analogues.begin(), analogues.end(),
                           [&analogue](const std::shared_ptr<Medicine>& existing)
                           {
                               return existing->getId() == analogue->getId();
                           });

    if (it == analogues.end())
        analogues.push_back(analogue);
    else
        throw std::invalid_argument("Analogue already exists: " + analogue->getId());
}

// Удаление аналога по ID
void Medicine::removeAnalogue(const std::string& analogueId)
{
    if (analogueId.empty())
        throw std::invalid_argument("Analogue ID cannot be empty");

    auto it = std::remove_if(analogues.begin(), analogues.end(),
                             [&analogueId](const std::shared_ptr<Medicine>& analogue)
                             {
                                 return analogue->getId() == analogueId;
                             });

    if (it != analogues.end())
        analogues.erase(it, analogues.end());
    else
        throw std::runtime_error("Analogue not found: " + analogueId);
}

// Получение всех аналогов
const std::vector<std::shared_ptr<Medicine>>& Medicine::getAnalogues() const
{
    return analogues;
}

// Поиск аналогов по действующему веществу
std::vector<std::shared_ptr<Medicine>> Medicine::findAnaloguesBySubstance(const std::string& substance) const
{
    if (substance.empty())
        throw std::invalid_argument("Substance cannot be empty");

    std::vector<std::shared_ptr<Medicine>> result;

    for (const auto& analogue : analogues)
    {
        if (analogue->getActiveSubstance() == substance)
            result.push_back(analogue);
    }

    return result;
}

// Проверка, является ли другое лекарство аналогом
bool Medicine::isAnalogueOf(const Medicine& other) const
{
    // Два лекарства считаются аналогами, если у них одинаковое действующее вещество
    // и они не являются одним и тем же лекарством
    return (this->getId() != other.getId()) &&
           (this->getActiveSubstance() == other.getActiveSubstance());
}

// Оператор присваивания
Medicine& Medicine::operator=(const Medicine& other)
{
    if (this != &other)
    {
        // Присваиваем базовую часть
        MedicalProduct::operator=(other);

        // Присваиваем собственные поля
        isPrescription = other.isPrescription;
        activeSubstance = other.activeSubstance;
        instructions = other.instructions;
        analogues = other.analogues; // Копируем shared_ptr
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Medicine& med)
{
    // Выводим базовую информацию
    os << static_cast<const MedicalProduct&>(med) << ";";

    // Выводим специфичную для Medicine информацию
    os << (med.isPrescription ? "Yes" : "No") << ";"
       << med.activeSubstance << ";"
       << med.instructions << ";"
       << med.getDosageForm() << ";"
       << med.getAdministrationMethod();

    return os;
}

std::istream& operator>>(std::istream& is, Medicine& med)
{
    try
    {
        std::string line;
        std::getline(is, line);

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        // Разбиваем строку по разделителю ';'
        while (std::getline(iss, token, ';')) {
            tokens.push_back(token);
        }

        // Проверяем, что получили все поля (базовые + специфичные)
        if (tokens.size() < 10) {
            throw InvalidProductDataException("medicine data", "invalid number of fields");
        }

        // Восстанавливаем базовую информацию из первых 5 полей
        std::istringstream baseStream(tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" + tokens[3] + ";" + tokens[4]);
        baseStream >> static_cast<MedicalProduct&>(med);

        // Заполняем специфичные поля Medicine
        // Prescription (6-е поле)
        std::istringstream prescriptionStream(tokens[5]);
        med.isPrescription = SafeInput::readBoolean(prescriptionStream, "Prescription status");

        // Active Substance (7-е поле)
        med.activeSubstance = tokens[6];
        if (med.activeSubstance.empty()) {
            throw InvalidProductDataException("Active substance", "cannot be empty");
        }

        // Instructions (8-е поле)
        med.instructions = tokens[7];
        if (med.instructions.empty()) {
            throw InvalidProductDataException("Instructions", "cannot be empty");
        }

        // Dosage Form и Administration Method (9-е и 10-е поля) - только для вывода

        // Очищаем аналоги (не сохраняем в потоковом вводе/выводе)
        med.analogues.clear();

    }
    catch (const PharmacyException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw InvalidProductDataException("medicine data", e.what());
    }

    return is;
}
