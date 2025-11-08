#ifndef PHARMACY_H
#define PHARMACY_H

#include "medicine.h"
#include "medicalproduct.h"
#include <vector>
#include <memory>
#include "storage.h"

class Pharmacy
{
private:
    std::string id;
    std::string name;  // Убрать const
    std::string address;
    std::string phoneNumber;
    double rentCost;

    Storage storage;

public:
    // Добавить параметр name в конструктор
    Pharmacy(const std::string& id, const std::string& name, const std::string& addr, const std::string& phone,
             double rent);
    Pharmacy();
    Pharmacy(const Pharmacy& other);
    ~Pharmacy() = default;

    // Управление складом
    void addToStorage(std::shared_ptr<MedicalProduct> product, int quantity);
    void removeFromStorage(const std::string& productId, int quantity);
    int checkStock(const std::string& productId) const;

    std::shared_ptr<MedicalProduct> findProduct(const std::string& productNameOrId) const;

    // Поиск аналогов
    std::vector<std::shared_ptr<Medicine>> findAvailableAnalogues(const std::string& medicineId) const;

    // Геттеры
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getAddress() const { return address; }
    std::string getPhoneNumber() const { return phoneNumber; }
    double getRentCost() const { return rentCost; }

    // Получение всех продуктов
    std::vector<std::pair<std::shared_ptr<MedicalProduct>, int>> getAllProducts() const;

    // Операторы
    Pharmacy& operator=(const Pharmacy& other);
    friend std::ostream& operator<<(std::ostream& os, const Pharmacy& pharmacy);
    friend std::istream& operator>>(std::istream& is, Pharmacy& pharmacy);
};

#endif
