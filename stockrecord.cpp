#include "stockrecord.h"
#include <sstream>

std::ostream& operator<<(std::ostream& os, const StockRecord& record)
{
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
        std::istringstream iss(line);
        std::string token;

        if (std::getline(iss, record.productId, ';') &&
            std::getline(iss, record.pharmacyId, ';') &&
            std::getline(iss, token, ';')) {

            record.quantity = std::stoi(token);

            std::string dateStr;
            if (std::getline(iss, dateStr)) {
                try {
                    record.receiptDate = SafeDate::fromString(dateStr);
                } catch (const std::exception& e) {
                    // Если не удалось распарсить дату, используем текущую дату
                    std::time_t t = std::time(nullptr);
                    std::tm* now = std::localtime(&t);
                    record.receiptDate = SafeDate(*now);
                }
            }
        }
    }
    return is;
}
