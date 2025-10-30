#include "medicine.h"
#include <algorithm>
#include <sstream>

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

// Оператор вывода
std::ostream& operator<<(std::ostream& os, const Medicine& med)
{
    // Выводим базовую информацию
    os << static_cast<const MedicalProduct&>(med);

    // Выводим специфичную для Medicine информацию
    os << "Prescription: " << (med.isPrescription ? "Yes" : "No") << "\n"
       << "Active Substance: " << med.activeSubstance << "\n"
       << "Instructions: " << med.instructions << "\n"
       << "Dosage Form: " << med.getDosageForm() << "\n"
       << "Administration Method: " << med.getAdministrationMethod() << "\n";

    // Выводим аналоги
    os << "Analogues (" << med.analogues.size() << "):\n";
    for (const auto& analogue : med.analogues) {
        os << "  - " << analogue->getName() << " (" << analogue->getId() << ")\n";
    }

    return os;
}

// Оператор ввода
std::istream& operator>>(std::istream& is, Medicine& med)
{
    // Читаем базовую информацию
    is >> static_cast<MedicalProduct&>(med);

    // Читаем специфичную для Medicine информацию
    std::string prescriptionStr;
    std::string temp;

    // Пропускаем метку
    std::getline(is, temp); // "Prescription: "
    std::getline(is, prescriptionStr);
    med.isPrescription = (prescriptionStr == "Yes");

    // Active Substance
    std::getline(is, temp); // "Active Substance: "
    std::getline(is, med.activeSubstance);

    // Instructions
    std::getline(is, temp); // "Instructions: "
    std::getline(is, med.instructions);

    // Пропускаем Dosage Form и Administration Method (они виртуальные)
    std::getline(is, temp); // "Dosage Form: "
    std::getline(is, temp); // значение
    std::getline(is, temp); // "Administration Method: "
    std::getline(is, temp); // значение

    // Очищаем аналоги (они будут устанавливаться отдельно)
    med.analogues.clear();

    // Пропускаем информацию об аналогах
    std::getline(is, temp); // "Analogues (X):"
    std::string line;
    while (std::getline(is, line))
    {
        if (line.empty() || line[0] != ' ')
        {
            // Если строка не начинается с пробела, это следующий раздел
            is.putback('\n');
            for (auto it = line.rbegin(); it != line.rend(); ++it)
            {
                is.putback(*it);
            }
            break;
        }
        // Пропускаем строки аналогов
    }

    return is;
}
