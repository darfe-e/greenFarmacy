#ifndef RETURN_H
#define RETURN_H

#include "inventoryoperation.h"

class Return : public InventoryOperation
{
private:
    std::string reason;

public:
    Return(std::string id, SafeDate date,
           std::shared_ptr<MedicalProduct> product, int quantity,
           std::string reason, std::string status = "pending");
    Return();
    Return(const Return& other);
    ~Return() override = default;

    std::string getOperationType() const override;
    void process() override;

    // Геттер
    std::string getReason() const { return reason; }

    // Операторы
    Return& operator=(const Return& other);
    friend std::ostream& operator<<(std::ostream& os, const Return& returnOp);
    friend std::istream& operator>>(std::istream& is, Return& returnOp); // исправлено: const убран
};

#endif
