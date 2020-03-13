/*
 * CSV.H --Comma-Separated-Values File processing API.
 *
 * Remarks:
 * This module provides an interface to "CSV" files as defined by RFC4180,
 * and used for the "text/csv" MIME media type.
 *
 * See Also:
 * https://tools.ietf.org/html/rfc4180
 */
#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include <xtd.h>
#include <symbol.h>
#include <vector.h>                    /* csv_parse_fields() returns vector */

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    enum CSVfileConsts
    {
        CSV_TEXT_MAX = 4096,           /* max 4K text per record */
    };
    typedef struct CSVField_t
    {
        Symbol item;
        const char *scan_fmt;          /* used by csv_read() */
        const char *print_fmt;         /* used by csv_write() */
    } CSVField, *CSVFieldPtr;

    typedef struct CSVFile_t
    {
        FILE *fp;
        char mode;                     /* "r", "w", "a" */
        size_t n_field;
        CSVFieldPtr field;             /* vector of fields */
    } CSVFile, *CSVFilePtr;

    CSVFilePtr csv_open(const char *path, const char *mode, ...);
    void csv_close(CSVFilePtr csv_fp);
    int csv_read(CSVFilePtr csv_fp, size_t n_values, Atom values[],
                 size_t n_bytes, char bytes[]);
    int csv_write(CSVFilePtr csv_fp, size_t n_values, Atom values[]);
    CSVFieldPtr csv_field(CSVFilePtr csv_fp, const char *name);
    CSVFieldPtr *csv_parse_fields(CSVFilePtr csv_fp, char *fields);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* CSV_H */
