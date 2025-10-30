#ifndef WRITEOFF_H
#define WRITEOFF_H

#include "inventoryoperation.h"

class WriteOff : public InventoryOperation
{
private:
    std::string writeOffReason;

public:
    WriteOff(std::string id, SafeDate date,
             std::shared_ptr<MedicalProduct> product, int quantity,
             std::string reason, std::string status = "pending");
    WriteOff();
    WriteOff(const WriteOff& other);
    ~WriteOff() override = default;

    std::string getOperationType() const override;
    void process() override;

    // Геттер
    std::string getWriteOffReason() const { return writeOffReason; }

    // Операторы
    WriteOff& operator=(const WriteOff& other);
    friend std::ostream& operator<<(std::ostream& os, const WriteOff& writeOff);
    friend std::istream& operator>>(std::istream& is, WriteOff& writeOff);
};

#endif
