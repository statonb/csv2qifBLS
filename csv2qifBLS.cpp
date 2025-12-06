#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include "mmSymbols.h"
#include "cusipBankMap.h"

#define MAX_LINE 4096
#define MAX_FIELDS  32

const char *SW_VERSION =    "1.04";
const char *SW_DATE =       "2025-12-06";

const char *DELIMITER_STRING =  ",";

#define COLLAPSE_FLAG   (0)

typedef enum
{
    UNKNOWN_BANK_FORMAT
    , BOA_FORMAT
    , CITI_FORMAT
    , FIDELITY_FORMAT
    , SCHWAB_BANK_FORMAT
    , SCHWAB_BROKERAGE_FORMAT
}   bankFormat_t;

// Remove surrounding quotes from a field, if present
void strip_quotes(char *s) {
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '"' && s[len-1] == '"') {
        memmove(s, s+1, len-2);
        s[len-2] = '\0';
    }
}

// Remove all quotes from a line.
// This will remove quotes from within a field
// so only do this when trying to find the
// column header line 
void remove_all_quotes(char *s) {
    char *dst = s, *src = s;
    while (*src) {
        if  (*src != '"')
        {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

// Remove all commas from a number field
void remove_commas_and_dollars(char *s) {
    char *dst = s, *src = s;
    while (*src) {
        if  (   (*src != ',')
             && (*src != '$')
            )
        {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

char *strcasestr_simple(const char *hay, const char *needle) {
    size_t nlen = strlen(needle);
    if (nlen == 0) return (char *)hay;
    for (; *hay; hay++) {
        if (tolower((unsigned char)*hay) == tolower((unsigned char)*needle)) {
            if (strncasecmp(hay, needle, nlen) == 0) return (char *)hay;
        }
    }
    return NULL;
}

// Parse a CSV line into fields handling quoted fields and empty fields.
// Returns number of fields parsed (up to max_fields).
int parse_csv_line(const char *line, char fields[][MAX_LINE], int max_fields) {
    int fi = 0;
    const char *p = line;

    while (*p != '\0' && fi < max_fields) {
        char *out = fields[fi];
        int o = 0;

        if (*p == '"') {
            // Quoted field
            p++; // skip opening quote
            while (*p != '\0') {
                if (*p == '"') {
                    // Handle escaped double quotes
                    if (*(p+1) == '"') {
                        out[o++] = '"';
                        p += 2;
                        continue;
                    }
                    p++; // closing quote
                    break;
                }
                out[o++] = *p++;
            }
            // Skip until comma or end
            while (*p != '\0' && *p != ',') p++;
            if (*p == ',') p++;
        } else {
            // Unquoted field
            while (*p != '\0' && *p != ',') {
                out[o++] = *p++;
            }
            if (*p == ',') p++;
        }

        out[o] = '\0';
        fi++;
    }

    // Handle trailing commas meaning empty fields
    int len = strlen(line);
    for (int i = len - 1; i >= 0; i--) {
        if (line[i] == ',') {
            if (fi < max_fields) {
                fields[fi][0] = '\0';
                fi++;
            }
        } else break;
    }

    return fi;
}

bankFormat_t string2bankFormat(const char *s)
{
    bankFormat_t    ret = UNKNOWN_BANK_FORMAT;

    if (strcasestr_simple(s, "boa"))
    {
        ret = BOA_FORMAT;
    }
    else if (strcasestr_simple(s, "citi"))
    {
        ret = CITI_FORMAT;
    }
    else if (strcasestr_simple(s, "fid"))
    {
        ret = FIDELITY_FORMAT;
    }
    else if (strcasestr_simple(s, "schwabbank"))
    {
        ret = SCHWAB_BANK_FORMAT;
    }
    else if (strcasestr_simple(s, "schwabbrok"))
    {
        ret = SCHWAB_BROKERAGE_FORMAT;
    }
    else
    {
        ret = UNKNOWN_BANK_FORMAT;
    }
    return ret;
}

void usage(const char *prog, const char *extraLine = (const char *)(NULL));

void usage(const char *prog, const char *extraLine)
{
    fprintf(stderr, "%s Ver %s %s\n", prog, SW_VERSION, SW_DATE);
    fprintf(stderr, "usage: %s <options>\n", prog);
    fprintf(stderr, "-i --input filename       input .csv file.\n");
    fprintf(stderr, "                          Extension will be added if not provided.\n");
    fprintf(stderr, "-o --output filename      output .qif file.\n");
    fprintf(stderr, "                          Filename will be generated from input filename\n");
    fprintf(stderr, "                          if not provided.\n");
    fprintf(stderr, "-f --format Bank          Different banks format CSV files differently.\n");
    fprintf(stderr, "                          Possible selections are as follows:\n");
    fprintf(stderr, "                             BoA\n");
    fprintf(stderr, "                             Citi\n");
    fprintf(stderr, "                             Fidelity\n");
    fprintf(stderr, "                             SchwabBank\n");
    fprintf(stderr, "                             SchwabBrokerage\n");
    fprintf(stderr, "-q --quiet                Quiet running (or decrease verbosity).\n");
    fprintf(stderr, "-v --verbose              Increase verbosity\n");
    if (extraLine) fprintf(stderr, "\n%s\n", extraLine);
}

void modifyCDDescription(char *desc, const char *bankName)
{
    if (strncasecmp(desc, "INTEREST", 8) == 0)
    {
        strcpy(desc, bankName);
        strcat(desc, " - Interest");
    }
    else if ((strncasecmp(desc, "REDEMPTION", 10) == 0))
    {
        strcpy(desc, bankName);
        strcat(desc, " - Redemption");
    }
}

void modifyMMDescription(char *desc, char *symbol)
{
    if  (   (strncasecmp(desc, "DIVIDEND", 8) == 0)
         || (strncasecmp(desc, "Reinvest Dividend", 17) == 0)
         || (strncasecmp(desc, "Cash Dividend", 13) == 0)
        )
    {
        strcpy(desc, symbol);
        strcat(desc, " Dividend");
    }
    else if (   (strncasecmp(desc, "REINVESTMENT", 12) == 0)
             || (strncasecmp(desc, "YOU BOUGHT", 10) == 0)
             || (strncasecmp(desc, "Reinvest Shares", 15) == 0)
             || (strncasecmp(desc, "Buy", 3) == 0)
            )
    {
        strcpy(desc, symbol);
        strcat(desc, " Purchase");
    }
    else if (   (strncasecmp(desc, "YOU SOLD", 8) == 0)
             || (strncasecmp(desc, "Sell", 4) == 0)
            )
    {
        strcpy(desc, symbol);
        strcat(desc, " Sale");
    }
}

void modifyTBillDescription(char *desc)
{
    if (strncasecmp(desc, "YOU BOUGHT", 10) == 0)
    {
        strcpy(desc, "T-Bill Purchase");
    }
    else if ((strncasecmp(desc, "REDEMPTION", 10) == 0))
    {
        strcpy(desc, "T-Bill Redemption");
    }
}

int main(int argc, char *argv[])
{
    int                 opt;
    char                inFileName[MAX_LINE];
    char                outFileName[MAX_LINE];
    bool                usageError = false;
    char                *cp;
    FILE                *fpIn;
    FILE                *fpOut;
    bool                inTransactionSection = false;
    char                line[MAX_LINE];
    char                date[MAX_LINE];
    char                amt[MAX_LINE];
    char                desc[MAX_LINE];
    char                symbol[MAX_LINE];
    char                cashBal[MAX_LINE];
    char                fields[MAX_FIELDS][MAX_LINE];
    int                 numTransactions = 0;
    double              withdrawModifier = 1.0;
    int                 verbosity = 1;
    bankFormat_t        bankFormat = UNKNOWN_BANK_FORMAT;
    MoneyMarketSymbols  mmSymbols;
    CUSIPBankMap        cusip2bank;

    inFileName[0] = '\0';
    outFileName[0] = '\0';

    struct option longOptions[] =
    {
        {"input",       required_argument,  0,      'i'}
        ,{"output",     required_argument,  0,      'o'}
        ,{"format",     required_argument,  0,      'f'}
        ,{"quiet",      no_argument,        0,      'q'}
        ,{"verbose",    no_argument,        0,      'v'}
        ,{0,0,0,0}
    };

    while (1)
    {
        int optionIndex = 0;
        opt = getopt_long(argc, argv, "i:o:f:qv", longOptions, &optionIndex);

        if (-1 == opt) break;

        switch (opt)
        {
        case 'i':
            strcpy(inFileName, optarg);
            break;
        case 'o':
            strcpy(outFileName, optarg);
            break;
        case 'f':
            bankFormat = string2bankFormat(optarg);
            break;
        case 'q':
            --verbosity;
            break;
        case 'v':
            ++verbosity;
            break;
        default:
            usageError = true;
            break;
        }
    }

    if (usageError)
    {
        usage(basename(argv[0]));
        return -1;
    }

    if (UNKNOWN_BANK_FORMAT == bankFormat)
    {
        usage(basename(argv[0]), "Unknown Bank Format");
        return -6;
    }
    else
    {
        printf("Bank Format: %d\n", (int)bankFormat);
    }

    // strcpy(inFileName, "/home/bruno/Downloads/schwab.csv");
    if ('\0' == inFileName[0])
    {
        usage(basename(argv[0]), "Input filename required");
        return -2;
    }

    cp = strchr(inFileName, '.');
    if ((char *)(NULL) == cp)
    {
        // No extension provided.  Add .csv
        strcat(inFileName, ".csv");
    }

    if ('\0' == outFileName[0])
    {
        // Create output file name from input file name
        strcpy(outFileName, inFileName);
        cp = strrchr(outFileName, '.');
        if ((char *)(NULL) == cp)
        {
            // Something went wrong because there should
            // definately be a '.' in the filename
            usage(basename(argv[0]), "Internal error with file names");
            return -3;
        }
        else
        {
            *cp = '\0';
            strcat(outFileName, ".qif");
        }
    }
    else
    {
        // An output file name was provided.
        // See if it has an extension
        cp = strchr(outFileName, '.');
        if ((char *)(NULL) == cp)
        {
            // No extension provided.  Add .qif
            strcat(outFileName, ".qif");
        }
    }

    fpIn = fopen(inFileName, "r");
    if ((FILE *)(NULL) == fpIn)
    {
        usage(basename(argv[0]), "Error opening input file");
        return -4;
    }

    fpOut = fopen(outFileName, "w");
    if ((FILE *)(NULL) == fpOut)
    {
        usage(basename(argv[0]), "Error opening output file");
        fclose(fpIn);
        return -5;
    }

    fprintf(fpOut, "!Type:Bank\n");

    while (fgets(line, sizeof(line), fpIn))
    {
        // Remove newline
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0') continue;

        if (false == inTransactionSection)
        {
            remove_all_quotes(line);
            switch (bankFormat)
            {
                case BOA_FORMAT:
                case SCHWAB_BANK_FORMAT:
                case SCHWAB_BROKERAGE_FORMAT:
                    if (strncmp(line, "Date,", 5) == 0) {
                        inTransactionSection = true;
                    }
                    break;
                case FIDELITY_FORMAT:
                    if (strncmp(line, "Run Date,", 9) == 0) {
                        inTransactionSection = true;
                    }
                    break;
                case CITI_FORMAT:
                    if (strncmp(line, "Status,", 6) == 0) {
                        inTransactionSection = true;
                    }
                    break;
                default:
                    usage(basename(argv[0]), "Internal error with bank format");
                    fclose(fpIn);
                    fclose(fpOut);
                    return -7;
                    break;
            }
            continue;
        }

        parse_csv_line(line, fields, MAX_FIELDS);

        //
        // Use the parse_csv_line results
        //
        if (BOA_FORMAT == bankFormat)
        {
            strcpy(date, fields[0]);
            strip_quotes(date);
            strcpy(desc, fields[1]);
            strip_quotes(desc);
            strcpy(amt, fields[2]);
        }
        else if (FIDELITY_FORMAT == bankFormat)
        {
            strcpy(cashBal, fields[15]);
            strip_quotes(cashBal);
            if (strncasecmp(cashBal, "Processing", 10) == 0) {
                // Skip transactions that are still in process
                continue;
            }
            strcpy(date, fields[0]);
            strip_quotes(date);
            if (isdigit(date[0]) == 0) {
                // Skip lines without a valid date
                continue;
            }
            strcpy(desc, fields[1]);
            strcpy(symbol, fields[2]);
            strcpy(amt, fields[14]);

            // Determine if the description needs to be modified
            strip_quotes(desc);
            strip_quotes(symbol);
            if (mmSymbols.contains(symbol)) {
                modifyMMDescription(desc, symbol);
            }
            else if (strncasecmp(symbol, "912797", 6) == 0) {
                modifyTBillDescription(desc);
            }
            else if (cusip2bank.contains(symbol)) {
                modifyCDDescription(desc, cusip2bank.getBankNameC(symbol));
            }
        }
        else if (CITI_FORMAT == bankFormat)
        {
            strcpy(date, fields[1]);
            strip_quotes(date);

            strcpy(desc, fields[2]);
            strip_quotes(desc);

            // This is the debit field in Citi.  It might be blank
            strcpy(amt, fields[3]);
            if (amt[0] == '\0')
            {
                strcpy(amt, fields[4]);     // Try the Credit field instead
                withdrawModifier = 1.0;
            }
            else
            {
                // Withdraw field had an entry.
                // Citi lists this as a positive number, but
                // QIF needs it to be negative.
                withdrawModifier = -1.0;
            }

        }
        else if (SCHWAB_BANK_FORMAT == bankFormat)
        {
            strcpy(date, fields[0]);
            strip_quotes(date);

            strcpy(desc, fields[4]);
            strip_quotes(desc);

            // This is the Withdraw filed in Schwab.  It might be blank
            strcpy(amt, fields[5]);
            if (amt[0] == '\0')
            {
                strcpy(amt, fields[6]);     // Try the Deposit field instead
                withdrawModifier = 1.0;
            }
            else
            {
                // Withdraw field had an entry.
                // Schwab lists this as a positive number, but
                // QIF needs it to be negative.
                withdrawModifier = -1.0;
            }

        }
        else if (SCHWAB_BROKERAGE_FORMAT == bankFormat)
        {
            strcpy(date, fields[0]);
            strip_quotes(date);
            // Remove any "as of ..." portion of this field
            cp = strstr(date, " as of");
            if (cp) *cp = '\0';

            strcpy(desc, fields[3]);
            strcpy(symbol, fields[2]);
            strcpy(amt, fields[7]);

            // Determine if the description needs to be modified
            strip_quotes(desc);
            strip_quotes(symbol);
            if (mmSymbols.contains(symbol)) {
                // Replace the description with the action
                strcpy(desc, fields[1]);
                strip_quotes(desc);
                modifyMMDescription(desc, symbol);
            }

        }

        strip_quotes(amt);
        remove_commas_and_dollars(amt);

        if (amt[0] == '\0') continue;

        double amtd = strtod(amt, NULL) * withdrawModifier;

        if  (   (verbosity >= 2)
             && (amt[0] != '\0')    // Wouldn't be here if amt was null, but check anyway
            )
        {
            printf("%s\t%.16s\t$%.2lf\n", date, desc, amtd);
        }

        fprintf(fpOut, "D%s\n", date);
        fprintf(fpOut, "P%s\n", desc);
        fprintf(fpOut, "T%.2lf\n", amtd);
        fprintf(fpOut, "C*\n");
        fprintf(fpOut, "^\n");
        ++numTransactions;
    }

    fclose(fpIn);
    fclose(fpOut);

    if (verbosity >= 1)
    {
        printf("Input File            : %s\n", inFileName);
        printf("Output File           : %s\n", outFileName);
        printf("Number of Transactions: %d\n", numTransactions);
    }


    return 0;

}
