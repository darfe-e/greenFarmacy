#ifndef STORAGE_H
#define STORAGE_H

#include "medicalproduct.h"
#include <map>
#include <vector>
#include <memory>

struct Storage
{
    std::map<std::string, std::pair<std::shared_ptr<MedicalProduct>, int>> items;

    void addProduct(std::shared_ptr<MedicalProduct> product, int quantity);
    void removeProduct(const std::string& productId, int quantity);
    int getQuantity(const std::string& productId) const;
    bool contains(const std::string& productId) const;
    std::vector<std::string> getAllProductIds() const;
};

#endif // STORAGE_H
