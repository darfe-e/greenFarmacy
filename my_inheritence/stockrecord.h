// stockrecord.h
#ifndef STOCKRECORD_H
#define STOCKRECORD_H

#include "safedate.h"
#include <string>
#include <iostream>

struct StockRecord
{
    std::string productId;    // ID препарата (только цифры, например "001")
    std::string pharmacyId;   // ID аптеки (с префиксом "Р", например "Р001")
    int quantity;            // Количество
    SafeDate receiptDate;    // Дата поступления

    StockRecord()
        : productId(""), pharmacyId(""), quantity(0), receiptDate(2000, 1, 1) {}

    StockRecord(const std::string& prodId, const std::string& pharmId,
                int qty, const SafeDate& date)
        : productId(prodId), pharmacyId(pharmId), quantity(qty), receiptDate(date) {}

    // Операторы для работы с File_text
    friend std::ostream& operator<<(std::ostream& os, const StockRecord& record);
    friend std::istream& operator>>(std::istream& is, StockRecord& record);
};

#endif // STOCKRECORD_H
