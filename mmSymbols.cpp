#include "mmSymbols.h"

bool MoneyMarketSymbols::contains(const std::string& symbol) const {
    return symbols.find(symbol) != symbols.end();
}

bool MoneyMarketSymbols::contains(const char* symbol) const {
    return symbols.find(symbol) != symbols.end();
}

// Usage:
// MoneyMarketSymbols mmSymbols;
// bool found = mmSymbols.contains("VUSXX");