#ifndef TABLET_H
#define TABLET_H

#include "medicalproduct.h"

class Tablet : public Medicine {
private:
    int pillCount; // Количество таблеток в упаковке
    double dosageMg; // Дозировка в мг

public:
    Tablet(const std::string& name, double price, const std::tm& expDate, bool prescription, const std::string& instr, int count, double dosage);
    void printInfo() const override; // Переопределяем метод вывода информации
};

#endif // TABLET_H
