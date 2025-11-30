#ifndef __MMSYMBOLS_H__
#define __MMSYMBOLS_H__

#include <unordered_set>
#include <string>

class MoneyMarketSymbols {
private:
    std::unordered_set<std::string> symbols;
    
public:
  MoneyMarketSymbols() {
        // Initialize your symbols
        symbols = {"FDLXX", "SPAXX", "FDRXX", "SPRXX", "FZFXX", "SNSXX", "SNVXX"};
    }  

    bool contains(const std::string& symbol) const;
    bool contains(const char* symbol) const;
};

// Usage:
// MoneyMarketSymbols mmSymbols;
// bool found = mmSymbols.contains("VUSXX");

#endif