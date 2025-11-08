#include "safedate.h"

SafeDate::SafeDate(int year, int month, int day)
{
    if (year < 1900) throw std::invalid_argument("Year must be >= 1900");
    if (month < 1 || month > 12) throw std::invalid_argument("Month must be between 1 and 12");
    if (day < 1 || day > 31) throw std::invalid_argument("Day must be between 1 and 31");

    date.tm_year = year - 1900;  // Год с 1900
    date.tm_mon = month - 1;     // Месяц 0-11
    date.tm_mday = day;          // День 1-31
    date.tm_hour = 0;            // Часы 0
    date.tm_min = 0;             // Минуты 0
    date.tm_sec = 0;             // Секунды 0
    date.tm_isdst = -1;          // Не учитывать летнее время

    std::tm dateCopy = date;
    if (std::mktime(&dateCopy) == -1) throw std::invalid_argument("Invalid date combination");

    if (date.tm_mday != dateCopy.tm_mday ||
        date.tm_mon != dateCopy.tm_mon ||
        date.tm_year != dateCopy.tm_year) throw std::invalid_argument("Invalid date (was normalized)");
}



SafeDate::SafeDate(const SafeDate& other)
{
    date.tm_year = other.date.tm_year;
    date.tm_mon = other.date.tm_mon;
    date.tm_mday = other.date.tm_mday;
    date.tm_hour = other.date.tm_hour;
    date.tm_min = other.date.tm_min;
    date.tm_sec = other.date.tm_sec;
    date.tm_isdst = other.date.tm_isdst;
}

SafeDate& SafeDate::operator=(const SafeDate& other)
{
    if (this != &other) {                                 // Проверка самоприсваивания
        date.tm_year = other.date.tm_year;
        date.tm_mon = other.date.tm_mon;
        date.tm_mday = other.date.tm_mday;
        date.tm_hour = other.date.tm_hour;
        date.tm_min = other.date.tm_min;
        date.tm_sec = other.date.tm_sec;
        date.tm_isdst = other.date.tm_isdst;
    }
    return *this;                                        // Возврат ссылки на текущий объект
}

SafeDate::SafeDate(const std::tm& tmDate) : date(tmDate)
{
    std::tm dateCopy = date;
    if (std::mktime(&dateCopy) == -1) throw std::invalid_argument("Invalid tm date structure");
}

bool SafeDate::isExpired() const
{
    std::time_t now = std::time(nullptr);
    std::tm dateCopy = date;
    std::time_t expTime = std::mktime(&dateCopy);
    return now > expTime;
}

std::string SafeDate::toString() const
{
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &date);
    return std::string(buffer);
}
