#include "safedate.h"
#include <sstream>

SafeDate::SafeDate()
{
    std::time_t t = std::time(nullptr);                   // Получение текущего времени
    std::tm* now = std::localtime(&t);                   // Преобразование в локальное время

    date.tm_year = now->tm_year;                         // Год (с 1900)
    date.tm_mon = now->tm_mon;                           // Месяц (0-11)
    date.tm_mday = now->tm_mday;                         // День (1-31)
    date.tm_hour = 0;                                    // Часы = 0
    date.tm_min = 0;                                     // Минуты = 0
    date.tm_sec = 0;                                     // Секунды = 0
    date.tm_isdst = -1;                                  // Автоматическое определение летнего времени
}

SafeDate::SafeDate(int year, int month, int day)
{
    if (year < 1900)                                     // Проверка года (минимум 1900)
        throw std::invalid_argument("Year must be >= 1900");
    if (month < 1 || month > 12)                         // Проверка месяца (1-12)
        throw std::invalid_argument("Month must be between 1 and 12");
    if (day < 1 || day > 31)                             // Проверка дня (1-31)
        throw std::invalid_argument("Day must be between 1 and 31");

    date.tm_year = year - 1900;                          // Преобразование года (год с 1900)
    date.tm_mon = month - 1;                             // Преобразование месяца (0-11)
    date.tm_mday = day;                                  // День месяца (1-31)
    date.tm_hour = 0;                                    // Часы = 0
    date.tm_min = 0;                                     // Минуты = 0
    date.tm_sec = 0;                                     // Секунды = 0
    date.tm_isdst = -1;                                  // Автоматическое определение летнего времени

    std::tm dateCopy = date;                             // Создание копии для проверки
    if (std::mktime(&dateCopy) == -1)                    // Проверка корректности даты
        throw std::invalid_argument("Invalid date combination");

    // Проверка, что дата не была нормализована (например, 31 февраля)
    if (date.tm_mday != dateCopy.tm_mday ||
        date.tm_mon != dateCopy.tm_mon ||
        date.tm_year != dateCopy.tm_year)
        throw std::invalid_argument("Invalid date (was normalized)");
}

SafeDate::SafeDate(const SafeDate& other)
{
    date.tm_year = other.date.tm_year;                   // Копирование года
    date.tm_mon = other.date.tm_mon;                     // Копирование месяца
    date.tm_mday = other.date.tm_mday;                   // Копирование дня
    date.tm_hour = other.date.tm_hour;                   // Копирование часов
    date.tm_min = other.date.tm_min;                     // Копирование минут
    date.tm_sec = other.date.tm_sec;                     // Копирование секунд
    date.tm_isdst = other.date.tm_isdst;                 // Копирование флага летнего времени
}

SafeDate& SafeDate::operator=(const SafeDate& other)
{
    if (this != &other)                                  // Проверка самоприсваивания
    {
        date.tm_year = other.date.tm_year;               // Копирование года
        date.tm_mon = other.date.tm_mon;                 // Копирование месяца
        date.tm_mday = other.date.tm_mday;               // Копирование дня
        date.tm_hour = other.date.tm_hour;               // Копирование часов
        date.tm_min = other.date.tm_min;                 // Копирование минут
        date.tm_sec = other.date.tm_sec;                 // Копирование секунд
        date.tm_isdst = other.date.tm_isdst;             // Копирование флага летнего времени
    }
    return *this;                                        // Возврат ссылки на текущий объект
}

SafeDate::SafeDate(const std::tm& tmDate) : date(tmDate)
{
    std::tm dateCopy = date;                             // Создание копии для проверки
    if (std::mktime(&dateCopy) == -1)                    // Проверка корректности структуры tm
        throw std::invalid_argument("Invalid tm date structure");
}

bool SafeDate::isExpired() const
{
    std::time_t now = std::time(nullptr);                // Получение текущего времени
    std::tm dateCopy = date;                             // Создание копии даты
    std::time_t expTime = std::mktime(&dateCopy);        // Преобразование даты в time_t
    return now > expTime;                                // Проверка истекла ли дата
}

std::string SafeDate::toString() const
{
    char buffer[20];                                     // Буфер для форматированной строки
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &date); // Форматирование даты
    return std::string(buffer);                          // Возврат строки
}

SafeDate SafeDate::fromString(const std::string& dateStr)
{
    int year, month, day;                                // Переменные для компонентов даты
    char dash1, dash2;                                   // Разделители

    std::istringstream iss(dateStr);                     // Создание строкового потока
    if (iss >> year >> dash1 >> month >> dash2 >> day)   // Парсинг строки
    {
        if (dash1 == '-' && dash2 == '-')                // Проверка формата разделителей
            return SafeDate(year, month, day);           // Создание объекта SafeDate
    }

    throw std::invalid_argument("Invalid date format: " + dateStr + ". Expected: YYYY-MM-DD");
}

SafeDate SafeDate::currentDate()
{
    return SafeDate();                                   // Использование конструктора по умолчанию
}
