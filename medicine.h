#ifndef MEDICINE_H
#define MEDICINE_H

#include "medicalproduct.h"
#include <iostream>

class Medicine : public MedicalProduct {
protected:
    bool isPrescription;
    std::string activeSubstance;
    std::string instructions;
    std::vector<std::string> analogues;

public:
    Medicine(const std::string& name, const std::string& manufacturer,
             const std::string& regNumber, double price, const std::tm& expDate,
             bool prescription, const std::string& activeSubst, const std::string& instr);

    virtual std::string getAdministrationMethod() const = 0; // Способ применения
    virtual std::string getDosageForm() const = 0; // Лекарственная форма

    bool isExpired() const;
    virtual void printInfo() const;
};

#endif // MEDICINE_H
