#include "storage.h"
#include "Exception/InventoryExceptions/InventoryException.h"
#include "Exception/InventoryExceptions/NegativeQuantityException.h"
#include "Exception/InventoryExceptions/InsufficientQuantityException.h"
#include <algorithm>
#include <stdexcept>

void Storage::addProduct(std::shared_ptr<MedicalProduct> product, int quantity)
{
    if (!product)                                                                // Проверка нулевого указателя
        throw InventoryException("Product cannot be null");
    if (quantity <= 0)                                                           // Проверка положительности количества
        throw NegativeQuantityException(quantity);

    std::string productId = product->getId();                                    // Получение ID продукта

    if (items.find(productId) != items.end())                                    // Если продукт уже есть на складе
        items[productId].second += quantity;                                     // Увеличение количества
    else                                                                         // Если продукта нет на складе
        items[productId] = std::make_pair(product, quantity);                    // Добавление нового продукта
}

void Storage::removeProduct(const std::string& productId, int quantity)
{
    if (productId.empty())                                                       // Проверка пустого ID продукта
        throw InventoryException("Product ID cannot be empty");
    if (quantity <= 0)                                                           // Проверка положительности количества
        throw NegativeQuantityException(quantity);

    auto it = items.find(productId);                                             // Поиск продукта по ID
    if (it == items.end())                                                       // Если продукт не найден
        throw InventoryException("Product not found: " + productId);

    int& currentQuantity = it->second.second;                                    // Ссылка на текущее количество

    if (quantity > currentQuantity)                                              // Если запрашиваемое количество больше имеющегося
        throw InsufficientQuantityException(productId, currentQuantity, quantity);

    currentQuantity -= quantity;                                                 // Уменьшение количества

    if (currentQuantity == 0)                                                    // Если количество стало нулевым
        items.erase(it);                                                         // Удаление продукта из хранилища
}

int Storage::getQuantity(const std::string& productId) const
{
    if (productId.empty())                                                       // Проверка пустого ID продукта
        throw InventoryException("Product ID cannot be empty");

    auto it = items.find(productId);                                             // Поиск продукта по ID
    if (it != items.end())                                                       // Если продукт найден
        return it->second.second;                                                // Возврат количества

    return 0;                                                                    // Продукт не найден, возврат 0
}

bool Storage::contains(const std::string& productId) const
{
    if (productId.empty())                                                       // Проверка пустого ID продукта
        throw InventoryException("Product ID cannot be empty");

    return items.find(productId) != items.end();                                 // Проверка существования продукта
}

std::vector<std::string> Storage::getAllProductIds() const
{
    std::vector<std::string> ids;                                                // Вектор для хранения ID
    ids.reserve(items.size());                                                   // Резервирование памяти

    for (const auto& item : items)                                               // Проход по всем элементам хранилища
        ids.push_back(item.first);                                               // Добавление ID в вектор

    return ids;                                                                  // Возврат списка ID
}
