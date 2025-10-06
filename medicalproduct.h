#ifndef MEDICALPRODUCT_H
#define MEDICALPRODUCT_H

#include <iostream>
#include <string>
#include <ctime>

class MedicalProduct {
protected:
    const std::string id;           // const поля
    const std::string name;
    const double basePrice;
    const std::tm expirationDate;
    const std::string manufacturerCountry;

public:
    MedicalProduct(const std::string& id, const std::string& name, double basePrice,
                   const std::tm& expDate, const std::string& country);

    MedicalProduct() = delete;

    MedicalProduct(const MedicalProduct& other);

    virtual ~MedicalProduct() = default;

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    double getBasePrice() const { return basePrice; }
    const std::tm& getExpirationDate() const { return expirationDate; }
    const std::string& getManufacturerCountry() const { return manufacturerCountry; }

    bool isExpired() const;

    friend std::ostream& operator<<(std::ostream& os, const MedicalProduct& prod);

    MedicalProduct& operator=(const MedicalProduct& other);

private:
    void validateId(const std::string& id) const;
    void validateName(const std::string& name) const;
    void validatePrice(double price) const;
    void validateCountry(const std::string& country) const;
    void validateExpirationDate(const std::tm& date) const;

    bool isValidDate(const std::tm& date) const;
};

#endif // MEDICALPRODUCT_H
