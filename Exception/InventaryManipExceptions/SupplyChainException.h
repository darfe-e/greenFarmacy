#ifndef SUPPLYCHAINEXCEPTION_H
#define SUPPLYCHAINEXCEPTION_H

#include "Exception/PharmacyExceptions/PharmacyException.h"

class SupplyChainException : public PharmacyException
{
private:
    std::string supplyId;

public:
    SupplyChainException(const std::string& id, const std::string& message)
        : PharmacyException("Supply '" + id + "': " + message),
        supplyId(id) {}

    std::string getErrorCode() const override { return "SUPPLY_CHAIN_001"; }
    std::string getSupplyId() const { return supplyId; }
};

#endif // SUPPLYCHAINEXCEPTION_H
