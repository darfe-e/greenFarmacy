#ifndef MEDICALPRODUCT_H
#define MEDICALPRODUCT_H

#include <iostream>
#include <string>
#include <ctime>
#include "safedate.h"
#include <iostream>
#include <sstream>    // для std::istringstream
#include <vector>     // для std::vector
#include <string>
#include <cctype>     // для std::isalnum
#include <limits>

class MedicalProduct
{
protected:
    std::string id;
    std::string name;
    double basePrice;
    SafeDate expirationDate;
    std::string manufacturerCountry;


public:
    MedicalProduct(std::string id, std::string name, double basePrice,
                   SafeDate expDate, std::string country);

    MedicalProduct();

    MedicalProduct(const MedicalProduct& other);

    virtual ~MedicalProduct() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    double getBasePrice() const { return basePrice; }
    std::string getExpirationDate() const { return expirationDate.toString(); }
    std::string getManufacturerCountry() const { return manufacturerCountry; }

    void setExpDate (SafeDate newExpDate){this->expirationDate = newExpDate;}
    void setId (std::string id){this->id = id;}

    bool isExpired() const {return expirationDate.isExpired();}

    friend std::ostream& operator<<(std::ostream& os, const MedicalProduct& prod);
    friend std::istream& operator >> (std::istream& is, MedicalProduct& prod);
    MedicalProduct& operator=(const MedicalProduct& other);


};

#endif // MEDICALPRODUCT_H
