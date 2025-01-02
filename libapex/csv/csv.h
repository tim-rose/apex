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
#ifndef APEX_CSV_H
#define APEX_CSV_H

#include <stdio.h>
#include <apex.h>
#include <apex/symbol.h>
#include <apex/vector.h>                /* csv_parse_fields() returns vector */

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    enum CSVfileConsts
    {
        CSV_TEXT_MAX = 4096,           /* max 4K text per record */
    };
    typedef struct CSVField
    {
        Symbol item;
        const char *scan_fmt;          /* used by csv_read() */
        const char *print_fmt;         /* used by csv_write() */
    } CSVField;

    typedef struct CSVFile
    {
        FILE *fp;
        char mode;                     /* "r", "w", "a" */
        size_t n_field;
        CSVField *field;             /* vector of fields */
    } CSVFile;

    CSVFile *csv_open(const char *path, const char *mode, ...);
    void csv_close(CSVFile *csv_fp);
    int csv_read(CSVFile *csv_fp, size_t n_values, Atom values[],
                 size_t n_bytes, char bytes[]);
    int csv_write(CSVFile *csv_fp, size_t n_values, Atom values[]);
    CSVField *csv_field(CSVFile *csv_fp, const char *name);
    CSVField **csv_parse_fields(CSVFile *csv_fp, char *fields);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_CSV_H */
