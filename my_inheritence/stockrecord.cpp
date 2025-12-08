#include "stockrecord.h"
#include <sstream>

std::ostream& operator<<(std::ostream& os, const StockRecord& record)
{
    // Формат: productId;pharmacyId;quantity;YYYY-MM-DD
    os << record.productId << ";"
       << record.pharmacyId << ";"
       << record.quantity << ";"
       << record.receiptDate.toString();
    return os;
}

std::istream& operator>>(std::istream& is, StockRecord& record)
{
    std::string line;
    if (std::getline(is, line)) {
        std::stringstream ss(line);
        std::string token;

        if (std::getline(ss, token, ';'))
            record.productId = token;

        if (std::getline(ss, token, ';'))
            record.pharmacyId = token;

        if (std::getline(ss, token, ';'))
            record.quantity = std::stoi(token);

        if (std::getline(ss, token, ';'))
        {
            // Парсим дату в формате YYYY-MM-DD
            int year, month, day;
            if (sscanf(token.c_str(), "%d-%d-%d", &year, &month, &day) == 3)
                record.receiptDate = SafeDate(year, month, day);
        }
    }
    return is;
}
