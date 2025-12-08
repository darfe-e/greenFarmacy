#ifndef SUPPLY_H
#define SUPPLY_H

#include "inventoryoperation.h"

class Supply : public InventoryOperation
{
private:
    std::string source;
    std::string destination;

public:
    Supply(std::string id, SafeDate date,
           std::shared_ptr<MedicalProduct> product, int quantity,
           std::string src, std::string dest,
           std::string status = "pending");
    Supply();
    Supply(const Supply& other);
    ~Supply() override = default;

    std::string getOperationType() const override;
    void process() override;

    // Геттеры
    std::string getSource() const { return source; }
    std::string getDestination() const { return destination; }

    // Операторы
    Supply& operator=(const Supply& other);
    friend std::ostream& operator<<(std::ostream& os, const Supply& supply);
    friend std::istream& operator>>(std::istream& is, Supply& supply);
};

#endif
