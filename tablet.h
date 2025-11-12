#ifndef TABLET_H
#define TABLET_H

#include "medicine.h"
#include <iostream>

class Tablet : public Medicine
{
private:
    int unitsPerPackage;
    double dosageMg;
    std::string coating;

public:
    Tablet(std::string id, std::string name, double basePrice,
           SafeDate expDate, std::string country,
           bool prescription, std::string activeSubst, std::string instr,
           int units, double dosage, std::string coating);
    //Tablet();
    Tablet(const Tablet& other);
    ~Tablet() override = default;

    std::string getAdministrationMethod() const override;
    std::string getDosageForm() const override;

    // Геттеры
    int getUnitsPerPackage() const { return unitsPerPackage; }
    double getDosageMg() const { return dosageMg; }
    std::string getCoating() const { return coating; }

    // Операторы
    Tablet& operator=(const Tablet& other);
    friend std::ostream& operator<<(std::ostream& os, const Tablet& tablet);
    friend std::istream& operator>>(std::istream& is, Tablet& tablet);
};

#endif
