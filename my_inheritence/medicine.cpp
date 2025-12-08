#include "medicine.h"
#include <algorithm>
#include <sstream>
#include "Exception/PharmacyExceptions/InvalidProductDataException.h"
#include "Exception/safeinput.h"

// Конструктор с параметрами
Medicine::Medicine(std::string id, std::string name, double basePrice,
                   SafeDate expDate, std::string country,
                   bool prescription, std::string activeSubst, std::string instr)
    : MedicalProduct(std::move(id), std::move(name), basePrice, std::move(expDate), std::move(country)),
    isPrescription(prescription),
    activeSubstance(std::move(activeSubst)),
    instructions(std::move(instr))
{
    // Валидация обязательных полей
    if (activeSubstance.empty())
        throw std::invalid_argument("Active substance cannot be empty");
    if (instructions.empty())
        throw std::invalid_argument("Instructions cannot be empty");

    // Валидация текстовых полей
    SafeInput::validateProductId(getId());
    SafeInput::validateTextField(getName(), "Name");
    SafeInput::validateTextField(getManufacturerCountry(), "Country");
    SafeInput::validateTextField(activeSubstance, "Active substance");
    SafeInput::validateTextField(instructions, "Instructions");

    // Валидация цены
    if (basePrice <= 0)
        throw InvalidProductDataException("Price", "must be positive");
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
    : MedicalProduct(other),                                 // Копирование базовой части
    isPrescription(other.isPrescription),                    // Копирование флага рецептурности
    activeSubstance(other.activeSubstance),                  // Копирование действующего вещества
    instructions(other.instructions),                        // Копирование инструкций
    analogues(other.analogues)                              // Поверхностное копирование указателей
{
}

// Добавление аналога
void Medicine::addAnalogue(std::shared_ptr<Medicine> analogue)
{
    if (!analogue)                                           // Проверка на нулевой указатель
        throw std::invalid_argument("Cannot add null analogue");

    if (analogue->getId() == this->getId())                  // Проверка на добавление самого себя
        throw std::invalid_argument("Medicine cannot be analogue of itself");

    // Поиск дубликата аналога
    auto it = std::find_if(analogues.begin(), analogues.end(),
                           [&analogue](const std::shared_ptr<Medicine>& existing)
                           {
                               return existing->getId() == analogue->getId();
                           });

    if (it == analogues.end())                               // Если аналог не найден
        analogues.push_back(analogue);                       // Добавляем новый аналог
    else
        throw std::invalid_argument("Analogue already exists: " + analogue->getId());
}

// Удаление аналога по ID
void Medicine::removeAnalogue(const std::string& analogueId)
{
    if (analogueId.empty())                                  // Проверка пустого ID
        throw std::invalid_argument("Analogue ID cannot be empty");

    // Перемещение удаляемых элементов в конец вектора
    auto it = std::remove_if(analogues.begin(), analogues.end(),
                             [&analogueId](const std::shared_ptr<Medicine>& analogue)
                             {
                                 return analogue->getId() == analogueId;
                             });

    if (it != analogues.end())                               // Если элементы для удаления найдены
        analogues.erase(it, analogues.end());                // Удаление элементов из вектора
    else
        throw std::runtime_error("Analogue not found: " + analogueId);
}

// Получение всех аналогов
const std::vector<std::shared_ptr<Medicine>>& Medicine::getAnalogues() const
{
    return analogues;                                        // Возврат константной ссылки на вектор
}

// Поиск аналогов по действующему веществу
std::vector<std::shared_ptr<Medicine>> Medicine::findAnaloguesBySubstance(const std::string& substance) const
{
    if (substance.empty())                                   // Проверка пустого вещества
        throw std::invalid_argument("Substance cannot be empty");

    std::vector<std::shared_ptr<Medicine>> result;           // Вектор для результатов

    for (const auto& analogue : analogues)                   // Проход по всем аналогам
    {
        if (analogue->getActiveSubstance() == substance)     // Сравнение действующих веществ
            result.push_back(analogue);                      // Добавление в результаты
    }

    return result;                                           // Возврат найденных аналогов
}

// Проверка, является ли другое лекарство аналогом
bool Medicine::isAnalogueOf(const Medicine& other) const
{
    // Аналоги должны иметь одинаковое действующее вещество и разные ID
    return (this->getId() != other.getId()) &&
           (this->getActiveSubstance() == other.getActiveSubstance());
}

// Оператор присваивания
Medicine& Medicine::operator=(const Medicine& other)
{
    if (this != &other)                                      // Проверка самоприсваивания
    {
        MedicalProduct::operator=(other);                    // Присваивание базовой части
        isPrescription = other.isPrescription;               // Копирование флага рецептурности
        activeSubstance = other.activeSubstance;             // Копирование действующего вещества
        instructions = other.instructions;                   // Копирование инструкций
        analogues = other.analogues;                         // Копирование вектора аналогов
    }
    return *this;                                            // Возврат текущего объекта
}

// Оператор вывода в поток
std::ostream& operator<<(std::ostream& os, const Medicine& med)
{
    os << static_cast<const MedicalProduct&>(med) << ";";    // Вывод базовой информации
    os << (med.isPrescription ? "Yes" : "No") << ";"         // Вывод флага рецептурности
       << med.activeSubstance << ";"                         // Вывод действующего вещества
       << med.instructions << ";"                            // Вывод инструкций
       << med.getDosageForm() << ";"                         // Вывод формы выпуска
       << med.getAdministrationMethod();                     // Вывод способа применения

    return os;                                               // Возврат потока
}

// Оператор ввода из потока
std::istream& operator>>(std::istream& is, Medicine& med)
{
    try
    {
        std::string line;
        std::getline(is, line);                              // Чтение строки из потока

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ';'))                // Разделение строки по ';'
            tokens.push_back(token);

        if (tokens.size() < 10)                              // Проверка количества полей
            throw InvalidProductDataException("medicine data", "invalid number of fields");

        // Восстановление базовой информации
        std::istringstream baseStream(tokens[0] + ";" + tokens[1] + ";" + tokens[2] + ";" + tokens[3] + ";" + tokens[4]);
        baseStream >> static_cast<MedicalProduct&>(med);

        // Чтение флага рецептурности
        std::istringstream prescriptionStream(tokens[5]);
        med.isPrescription = SafeInput::readBoolean(prescriptionStream, "Prescription status");

        // Чтение действующего вещества
        med.activeSubstance = tokens[6];
        if (med.activeSubstance.empty())
            throw InvalidProductDataException("Active substance", "cannot be empty");

        // Чтение инструкций
        med.instructions = tokens[7];
        if (med.instructions.empty())
            throw InvalidProductDataException("Instructions", "cannot be empty");

        med.analogues.clear();                               // Очистка аналогов
    }
    catch (const PharmacyException&)                         // Переброс исключений PharmacyException
    {
        throw;
    }
    catch (const std::exception& e)                          // Обработка других исключений
    {
        throw InvalidProductDataException("medicine data", e.what());
    }

    return is;                                               // Возврат потока
}

// Добавление аналога по ID
void Medicine::addAnalogueById(const std::string& analogueId,
                               const std::vector<std::shared_ptr<Medicine>>& allMedicines)
{
    if (analogueId == this->getId())                         // Проверка на добавление самого себя
        throw std::invalid_argument("Medicine cannot be analogue of itself");

    // Поиск лекарства по ID в общем списке
    auto it = std::find_if(allMedicines.begin(), allMedicines.end(),
                           [&analogueId](const std::shared_ptr<Medicine>& med)
                           {
                               return med->getId() == analogueId;
                           });

    if (it != allMedicines.end())                            // Если лекарство найдено
        addAnalogue(*it);                                    // Добавление аналога
    else
        throw std::runtime_error("Medicine with ID " + analogueId + " not found");
}

// Получение ID всех аналогов
std::vector<std::string> Medicine::getAnalogueIds() const
{
    std::vector<std::string> ids;                            // Вектор для ID
    for (const auto& analogue : analogues)                   // Проход по всем аналогам
        ids.push_back(analogue->getId());                    // Добавление ID в вектор
    return ids;                                              // Возврат списка ID
}

// Очистка всех аналогов
void Medicine::clearAnalogues()
{
    analogues.clear();                                       // Очистка вектора аналогов
}
