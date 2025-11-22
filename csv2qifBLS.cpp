#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include "stctok.h"

#define MAX_LINE 4096
#define MAX_FIELDS  16

const char *SW_VERSION =    "1.00";
const char *SW_DATE =       "2025-11-22";

const char *DELIMITER_STRING =  ",";

#define COLLAPSE_FLAG   (0)

// Remove surrounding quotes from a field, if present
void strip_quotes(char *s) {
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '"' && s[len-1] == '"') {
        memmove(s, s+1, len-2);
        s[len-2] = '\0';
    }
}

// Remove all commas from a number field
void remove_commas(char *s) {
    char *dst = s, *src = s;
    while (*src) {
        if (*src != ',') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
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
    fprintf(stderr, "-q --quiet                Quiet running (or decrease verbosity).\n");
    fprintf(stderr, "-v --verbose              Increase verbosity\n");
    if (extraLine) fprintf(stderr, "\n%s\n", extraLine);
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
    char                *ptr;
    char                date[MAX_LINE];
    char                amt[MAX_LINE];
    char                desc[MAX_LINE];
    char                fields[MAX_FIELDS][MAX_LINE];
    int                 numTransactions = 0;
    int                 verbosity = 1;

    inFileName[0] = '\0';
    outFileName[0] = '\0';

    struct option longOptions[] =
    {
        {"input",       required_argument,  0,      'i'}
        ,{"output",     required_argument,  0,      'o'}
        ,{"quiet",      no_argument,        0,      'q'}
        ,{"verbose",    no_argument,        0,      'v'}
        ,{0,0,0,0}
    };

    while (1)
    {
        int optionIndex = 0;
        opt = getopt_long(argc, argv, "i:o:qv", longOptions, &optionIndex);

        if (-1 == opt) break;

        switch (opt)
        {
        case 'i':
            strcpy(inFileName, optarg);
            break;
        case 'o':
            strcpy(outFileName, optarg);
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

    // strcpy(inFileName, "/home/bruno/Downloads/stmt.csv");
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
            if (strncmp(line, "Date,", 5) == 0) {
                inTransactionSection = true;
            }
            continue;
        }

        parse_csv_line(line, fields, MAX_FIELDS);

        // This is my way using stctok but it doesn't handle commas within quoted fields
        ptr = line;
        ptr = stctok(ptr, date, sizeof(date), (char *)DELIMITER_STRING, COLLAPSE_FLAG);
        ptr = stctok(ptr, desc, sizeof(desc), (char *)DELIMITER_STRING, COLLAPSE_FLAG);
        ptr = stctok(ptr, amt, sizeof(amt), (char *)DELIMITER_STRING, COLLAPSE_FLAG);
        //
        // Use the parse_csv_line results
        //
        strcpy(date, fields[0]);
        strcpy(desc, fields[1]);
        strcpy(amt, fields[2]);

        strip_quotes(date);
        strip_quotes(desc);
        strip_quotes(amt);

        if  (   (verbosity >= 2)
             && (amt[0] != '\0')
            )
        {
            printf("%s\t%.16s\t$%s\n", date, desc, amt);
        }

        remove_commas(amt);

        if (amt[0] == '\0') continue;

        fprintf(fpOut, "D%s\n", date);
        fprintf(fpOut, "P%s\n", desc);
        fprintf(fpOut, "T%s\n", amt);
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
