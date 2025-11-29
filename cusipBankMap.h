#ifndef __CUSIPBANKMAP_H__
#define __CUSIPBANKMAP_H__

#include <unordered_map>
#include <string>
#include <optional>

class CUSIPBankMap {
private:
    std::unordered_map<std::string, std::string> cusipToBank;
    
public:
    CUSIPBankMap() {
        // Initialize CUSIP to Bank Name mappings
        cusipToBank = {
            {"00351DAF3", "ABINGTON BANK"},
            {"028501AL8", "THE AMERICAN NB OF TERRELL TEXAS"},
            {"05580A7E8", "BMW BK NORTH AMER UTAH"},
            {"05600XNV8", "BMO HARRIS BANK NA"},
            {"06051XBM3", "BANK OF AMERICA"},
            {"06251A5Q9", "BANK HAPOALIM B M NEW YORK"},
            {"06251A6K1", "BANK HAPOALIM B M NEW YORK"},
            {"06405VFU8", "BANK NEW YORK MELLONCORP"},
            {"06418CHY5", "BANK OZK LITTLE ROCKARK"},
            {"06652GAX0", "BANKSOUTH Greensboro GA"},
            {"10421AAD9", "Bradesco Bank FL"},
            {"12143PBB2", "Burling Bank"},
            {"13005CCU3", "CALIFORNIA BANK OF COMMERCE"},
            {"15721UHG7", "CFBANK"},
            {"15721UHY8", "CFBANK"},
            {"15987UBH0", "Charles Schwab Bank, TX"},
            {"15987UBU1", "Charles Schwab Bank, TX"},
            {"173541AB9", "THE CITIZENS BK OF CLOVIS"},
            {"17801DGN0", "CITY NATL BK FLA MIA"},
            {"200339FS3", "Comerica Bank TX"},
            {"20415QJG5", "COMMUNITY WEST BK GOLETA"},
            {"22209WAA0", "COULEE BANK"},
            {"22209WAB8", "COULEE BANK"},
            {"227563DX8", "CROSS RIV BK TEANECK NJ"},
            {"23062KFX5", "Cumberland Fed Bank FSB Wis"},
            {"23062KGA4", "Cumberland Fed Bank FSB Wis"},
            {"23204HNU8", "Customers Bank PA"},
            {"2546736D3", "Discover Bank DE"},
            {"25665QBQ7", "DOLLAR BK FED SVGS BK PITTSBUR"},
            {"29367SKA1", "Enterprise Bank & Tr MO"},
            {"29367SLQ5", "ENTERPRISE BANK & TRUST"},
            {"300185MX0", "EVERGREEN BK GROUP ILL"},
            {"300185NB7", "EVERGREEN BK GROUP ILL"},
            {"31944MCU7", "FIRST CAROLINA BANK"},
            {"31944MCZ6", "FIRST CAROLINA BK ROCKY MT"},
            {"31983VDE4", "First Community Bank"},
            {"320165LF8", "FIRST FMRS B&T CONVERSE IND"},
            {"32016KBC3", "FIRST FARMERS & MERCHANTS BK"},
            {"32017MAT2", "FIRST FED BK PORT ANGELES WASH"},
            {"32026UJ31", "FIRST FNDTN BK IRVINE CA"},
            {"32026UQ82", "FIRST FNDTN BK IRVINE CA"},
            {"321089DH4", "First Ntnl Bank of P PA"},
            {"32108HAA4", "1ST NATIONAL BANK LEBANON OH"},
            {"32111LDH2", "FNB Sioux Falls"},
            {"32117BFW3", "First Natl BK Damariscotta ME"},
            {"32117BGW2", "First Natl BK Damariscotta ME"},
            {"332135KT8", "FIRST NATL BK OMAHA NEB"},
            {"337158BB4", "First Horizon Bank TN"},
            {"34387AFX2", "Flushing Bank"},
            {"35637RDW4", "Freedom Financial Bank Des Moines IA"},
            {"35909FAM6", "FRONTIER BK SIOUX FALLS SD"},
            {"366526BJ9", "GARRETT ST BK IND"},
            {"37173RAV5", "GENESEE REGL RCHSTER NY"},
            {"38150VFM6", "Goldman Sachs Bank U NY"},
            {"38150VQ44", "GOLDMAN SACHS BK USACD"},
            {"46593LW96", "JPMORGAN CHASE BK"},
            {"47804GLR2", "JOHN MARSHALL BANK"},
            {"48714LCT4", "KEARNY BK NEW JERSEY"},
            {"489265DR6", "Kennebec Savings Ban ME"},
            {"49306SJ98", "KEYBANK NATIONAL ASSOCIATION"},
            {"53724CAG2", "LITTLE HORN STATE BANK"},
            {"5380362A1", "LIVE OAK BKG CO NC"},
            {"538036D43", "LIVE OAK BKG CO NC"},
            {"538036Y57", "LIVE OAK BKG CO NC"},
            {"55316CBZ8", "M1 BANK (MISSOURI)"},
            {"588493PP5", "Merchants Bank of In IN"},
            {"58958PLS1", "Meridian Corp"},
            {"59013KWM4", "Merrick Bank"},
            {"61690DDB1", "Morgan Stanley Bank UT"},
            {"61690U3Y4", "Morgan Stanley Bank UT"},
            {"61768ERL9", "MORGAN STANLEY PVT B NY"},
            {"62847HDB2", "MutualOne Bank MA"},
            {"62847NDU7", "MVB BANK"},
            {"654062ME5", "NICOLET NATL BK GREEN BAY WI"},
            {"66405SEZ9", "NORTHEAST BANK"},
            {"669331AR1", "Norway Savings Bank ME"},
            {"67523TCQ4", "OceanFirst Bank, Ntn"},
            {"68622BAM2", "Origin Bank LA"},
            {"740367RM1", "PREFERRED BANK"},
            {"74277ABA5", "PRISM BANK"},
            {"75942DAW4", "Reliabank Dakota SD"},
            {"795234BF9", "NBT BK NA NORWICH NY"},
            {"82669LKN2", "SIGNATURE BANK OF ARKANSAS"},
            {"82869ADC6", "Simmons Bank AR"},
            {"83407DBC9", "SOFI BANK NATIONAL ASSOCIATION"},
            {"843879FT9", "SOUTHERN STATES BANK"},
            {"84464PBV8", "SOUTHPOINT BANK"},
            {"85528WEM1", "STARION BANK"},
            {"87220LBR4", "TBK Bank, SSB TX"},
            {"89155MCA3", "TOUCHMARK NATL BK NORCROSS GA"},
            {"89580DBC5", "TRIAD BUSINESS BANK"},
            {"898401DD3", "Trustmark Ntnl Bank MS"},
            {"90354KCD8", "US BK NATL ASSN"},
            {"90355GQH2", "UBS BK USA SALT LAKE CITY UT"},
            {"904198BX5", "UMB BK NATL ASSN KANS CITY MO"},
            {"910286GE7", "UNITED FIDELITY BANK"},
            {"913109AK0", "UNITED TR BK PALOS HEIGHTS"},
            {"919853HZ7", "Valley Ntnl Bancorp NJ"},
            {"92023LBS1", "VALLIANCE BANK"},
            {"92237VBD4", "VAST BANK NA"},
            {"923450FU9", "VERITEX CMNTY BK NA DALLAS TX"},
            {"949764CX0", "Wells Fargo Bank, Nt SD"},
            {"949764ED2", "Wells Fargo Bank, Nt SD"},
            {"949764FB5", "WELLS FARGO BANK NA"},
            {"95763PUF6", "WESTERN ALLIANCE BK PHOENIX"},
            {"971795ST7", "WILMINGTON SAVINGS FUND FSB"},
            {"98970LFC2", "Zions BanCorp Ntnl UT"}
        };
    }
    
    // Get bank name by CUSIP, returns nullptr if not found
    const std::string* getBankName(const std::string& cusip) const;
    
    // Overload for const char*
    const std::string* getBankName(const char* cusip) const;
    
    // Return a C style char*
    const char *getBankNameC(const char* cusip) const;

    // Alternative: using std::optional (C++17)
    std::optional<std::string> getBankNameOpt(const std::string& cusip) const;
    
    // Check if CUSIP exists
    bool contains(const std::string& cusip) const;
    
    bool contains(const char* cusip) const;
};

// Usage example:
// CUSIPBankMap cusipMap;
// const std::string* bankName = cusipMap.getBankName("00351DAF3");
// if (bankName) {
//     std::cout << "Bank: " << *bankName << std::endl;
// }

#endif