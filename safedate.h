#ifndef SAFEDATE_H
#define SAFEDATE_H

#include <ctime>
#include <stdexcept>
#include <string>

class SafeDate
{
private:
    std::tm date;

public:
    SafeDate(int year, int month, int day);
    SafeDate(const std::tm& tmDate);

    SafeDate (const SafeDate& other);
    SafeDate& operator= (const SafeDate& other);

    const std::tm& getTm() const { return date; }
    bool isExpired() const;

    std::string toString() const;

    int getYear() const { return date.tm_year + 1900; }
    int getMonth() const { return date.tm_mon + 1; }
    int getDay() const { return date.tm_mday; }
    static SafeDate fromString(const std::string& dateStr);

    static SafeDate currentDate();
    SafeDate();
};

#endif // SAFEDATE_H
