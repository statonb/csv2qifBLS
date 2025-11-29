#include "cusipBankMap.h"
    
// Get bank name by CUSIP, returns nullptr if not found
const std::string* CUSIPBankMap::getBankName(const std::string& cusip) const {
    auto it = cusipToBank.find(cusip);
    if (it != cusipToBank.end()) {
        return &(it->second);
    }
    return nullptr;
}

// Overload for const char*
const std::string* CUSIPBankMap::getBankName(const char* cusip) const {
    return getBankName(std::string(cusip));
}

// Return a C style char*
const char *CUSIPBankMap::getBankNameC(const char* cusip) const {
    return getBankName(std::string(cusip))->c_str();
}

// Alternative: using std::optional (C++17)
std::optional<std::string> CUSIPBankMap::getBankNameOpt(const std::string& cusip) const {
    auto it = cusipToBank.find(cusip);
    if (it != cusipToBank.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Check if CUSIP exists
bool CUSIPBankMap::contains(const std::string& cusip) const {
    return cusipToBank.find(cusip) != cusipToBank.end();
}

bool CUSIPBankMap::contains(const char* cusip) const {
    return contains(std::string(cusip));
}

// Usage example:
// CUSIPBankMap cusipMap;
// const std::string* bankName = cusipMap.getBankName("00351DAF3");
// if (bankName) {
//     std::cout << "Bank: " << *bankName << std::endl;
// }