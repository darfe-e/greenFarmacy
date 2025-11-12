#include "storage.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/InventoryExceptions/NegativeQuantityException.h"
#include "Exception/InventoryExceptions/InsufficientQuantityException.h"
#include <algorithm>
#include <stdexcept>

void Storage::addProduct(std::shared_ptr<MedicalProduct> product, int quantity)
{
    if (!product)
        throw InventoryException("Product cannot be null");
    if (quantity <= 0)
        throw NegativeQuantityException(quantity);

    std::string productId = product->getId();

    if (items.find(productId) != items.end())
        items[productId].second += quantity;
    else
        items[productId] = std::make_pair(product, quantity);
}

void Storage::removeProduct(const std::string& productId, int quantity)
{
    if (productId.empty())
        throw InventoryException("Product ID cannot be empty");
    if (quantity <= 0)
        throw NegativeQuantityException(quantity);

    auto it = items.find(productId);
    if (it == items.end())
        throw InventoryException("Product not found: " + productId);

    int& currentQuantity = it->second.second;

    if (quantity > currentQuantity)
        throw InsufficientQuantityException(productId, currentQuantity, quantity);

    currentQuantity -= quantity;

    if (currentQuantity == 0)
        items.erase(it);
}

int Storage::getQuantity(const std::string& productId) const
{
    if (productId.empty())
        throw InventoryException("Product ID cannot be empty");

    auto it = items.find(productId);
    if (it != items.end())
        return it->second.second;

    return 0;
}

bool Storage::contains(const std::string& productId) const
{
    if (productId.empty())
        throw InventoryException("Product ID cannot be empty");

    return items.find(productId) != items.end();
}

std::vector<std::string> Storage::getAllProductIds() const
{
    std::vector<std::string> ids;
    ids.reserve(items.size());

    for (const auto& item : items)
        ids.push_back(item.first);

    return ids;
}
