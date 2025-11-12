#ifndef SYRUP_H
#define SYRUP_H

#include "medicine.h"

class Syrup : public Medicine
{
private:
    double volumeMl;
    bool hasSugar;
    std::string flavor;

public:
    Syrup(std::string id, std::string name, double basePrice,
          SafeDate expDate, std::string country,
          bool prescription, std::string activeSubst, std::string instr,
          double volume, bool sugar, std::string flavor);
    //Syrup();
    Syrup(const Syrup& other);
    ~Syrup() override = default;

    std::string getAdministrationMethod() const override;
    std::string getDosageForm() const override;

    // Геттеры
    double getVolumeMl() const { return volumeMl; }
    bool getHasSugar() const { return hasSugar; }
    std::string getFlavor() const { return flavor; }

    // Операторы
    Syrup& operator=(const Syrup& other);
    friend std::ostream& operator<<(std::ostream& os, const Syrup& syrup);
    friend std::istream& operator>>(std::istream& is, Syrup& syrup);
};

#endif
