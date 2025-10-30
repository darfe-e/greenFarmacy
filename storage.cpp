#include "storage.h"
#include <algorithm>
#include <stdexcept>

void Storage::addProduct(std::shared_ptr<MedicalProduct> product, int quantity)
{
    if (!product)
        throw std::invalid_argument("Product cannot be null");
    if (quantity <= 0)
        throw std::invalid_argument("Quantity must be positive: " + std::to_string(quantity));

    std::string productId = product->getId();

    if (items.find(productId) != items.end())
        items[productId].second += quantity;                            // Увеличиваем количество существующего продукта
    else
        items[productId] = std::make_pair(product, quantity);           // Добавляем новый продукт
}

void Storage::removeProduct(const std::string& productId, int quantity)
{
    if (productId.empty())
        throw std::invalid_argument("Product ID cannot be empty");
    if (quantity <= 0)
        throw std::invalid_argument("Quantity must be positive: " + std::to_string(quantity));

    auto it = items.find(productId);
    if (it == items.end())
        throw std::runtime_error("Product not found: " + productId);

    int& currentQuantity = it->second.second;

    if (quantity > currentQuantity)
        throw std::runtime_error("Not enough quantity. Available: " +
                                 std::to_string(currentQuantity) +
                                 ", requested: " + std::to_string(quantity));

    currentQuantity -= quantity;                                        // Уменьшаем количество

    if (currentQuantity == 0)
        items.erase(it);                                                // Удаляем продукт если количество 0
}

int Storage::getQuantity(const std::string& productId) const
{
    if (productId.empty())
        throw std::invalid_argument("Product ID cannot be empty");

    auto it = items.find(productId);
    if (it != items.end())
        return it->second.second;                                       // Возвращаем количество

    return 0;                                                           // Продукт не найден
}

bool Storage::contains(const std::string& productId) const
{
    if (productId.empty())
        throw std::invalid_argument("Product ID cannot be empty");

    return items.find(productId) != items.end();                        // Проверка наличия
}

std::vector<std::string> Storage::getAllProductIds() const
{
    std::vector<std::string> ids;
    ids.reserve(items.size());                                          // Резервируем память

    for (const auto& item : items)
        ids.push_back(item.first);                                      // Добавляем ID продукта

    return ids;
}
