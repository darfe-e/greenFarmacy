#ifndef OINTMENT_H
#define OINTMENT_H

#include "medicine.h"

class Ointment : public Medicine
{
private:
    double weightG;
    std::string baseType;

public:
    Ointment(std::string id, std::string name, double basePrice,
             SafeDate expDate, std::string country,
             bool prescription, std::string activeSubst, std::string instr,
             double weight, std::string base);
    Ointment(){};
    Ointment(const Ointment& other);
    ~Ointment() override = default;

    std::string getAdministrationMethod() const override;
    std::string getDosageForm() const override;

    // Геттеры
    double getWeightG() const { return weightG; }
    std::string getBaseType() const { return baseType; }

    // Операторы
    Ointment& operator=(const Ointment& other);
    friend std::ostream& operator<<(std::ostream& os, const Ointment& ointment);
    friend std::istream& operator>>(std::istream& is, Ointment& ointment);
};

#endif
