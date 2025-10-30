#ifndef INVENTORYOPERATION_H
#define INVENTORYOPERATION_H

#include "medicalproduct.h"
#include "safedate.h"
#include <memory>

class InventoryOperation
{
protected:
    std::string id;
    SafeDate operationDate;
    std::shared_ptr<MedicalProduct> product;
    int quantity;
    std::string status;

public:
    InventoryOperation(std::string id, SafeDate date,
                       std::shared_ptr<MedicalProduct> product, int quantity,
                       std::string status = "pending");
    InventoryOperation();
    InventoryOperation(const InventoryOperation& other);
    virtual ~InventoryOperation() = default;

    // Чисто виртуальные методы
    virtual std::string getOperationType() const = 0;
    virtual void process() = 0;

    // Геттеры
    std::string getId() const { return id; }
    SafeDate getOperationDate() const { return operationDate; }
    std::string getProductId() const { return product->getId(); }
    int getQuantity() const { return quantity; }
    std::string getStatus() const { return status; }

    // Сеттер
    void setStatus(std::string newStatus) { status = newStatus; }

    // Операторы
    InventoryOperation& operator=(const InventoryOperation& other);
    friend std::ostream& operator<<(std::ostream& os, const InventoryOperation& operation);
    friend std::istream& operator>>(std::istream& is, InventoryOperation& operation);
};

#endif
