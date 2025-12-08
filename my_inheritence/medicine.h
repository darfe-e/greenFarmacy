#ifndef MEDICINE_H
#define MEDICINE_H

#include "medicalproduct.h"
#include <vector>
#include <memory>
#include <string>

class Medicine : public MedicalProduct
{
protected:
    bool isPrescription;
    std::string activeSubstance;
    std::string instructions;
    std::vector<std::shared_ptr<Medicine>> analogues;

public:
    Medicine(std::string id, std::string name, double basePrice,
             SafeDate expDate, std::string country,
             bool prescription, std::string activeSubst, std::string instr);

    Medicine();
    Medicine(const Medicine& other);

    virtual ~Medicine() = default;

    // Чисто виртуальные методы
    virtual std::string getAdministrationMethod() const = 0;
    virtual std::string getDosageForm() const = 0;

    // Методы для работы с аналогами
    void addAnalogue(std::shared_ptr<Medicine> analogue);
    void removeAnalogue(const std::string& analogueId);
    const std::vector<std::shared_ptr<Medicine>>& getAnalogues() const;
    void addAnalogueById(const std::string& analogueId,
                         const std::vector<std::shared_ptr<Medicine>>& allMedicines);
    std::vector<std::string> getAnalogueIds() const;
    void clearAnalogues();

    // Поиск аналогов
    std::vector<std::shared_ptr<Medicine>> findAnaloguesBySubstance(const std::string& substance) const;
    bool isAnalogueOf(const Medicine& other) const;

    // Геттеры
    bool getIsPrescription() const { return isPrescription; }
    std::string getActiveSubstance() const { return activeSubstance; }
    std::string getInstructions() const { return instructions; }

    // Операторы
    Medicine& operator=(const Medicine& other);
    friend std::ostream& operator<<(std::ostream& os, const Medicine& med);
    friend std::istream& operator>>(std::istream& is, Medicine& med);
};

#endif
