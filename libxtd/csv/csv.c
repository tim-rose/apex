/*
 * CSV.C --Routines for doing I/O on Comma-Separated Values.
 *
 * Contents:
 * debug_fields_()     --Log field information at debug level.
 * csv_strip_eol_()    --Strip common newline characters from a string.
 * csv_mk_header_()    --Construct/malloc a header record from text.
 * csv_cmp_header_()   --Compare a file's header with a prepared header.
 * csv_write_header_() --Write the CSV file header.
 * csv_open()          --Open a CSV file.
 * csv_close()         --Close a fully/partially open CSV file.
 * csv_read()          --Read a record from the CSV file.
 * csv_write()         --Write a record to the CSV file.
 * csv_field()         --Find a CSV file's field by name.
 * parse_fields()      --Parse a list of names into a vector of CSVFieldPtrs.
 *
 * Remarks:
 * This module provides a simple open/close/read/write API for
 * CSV files.  Note that there's no seek (yet), as CSV files
 * are typically read in their entirety, and only appended to.
 *
 * The CSV processing is currently a little simplistic: it doesn't
 * handle any quoting or escaping in the manner of Excel.
 *
 * See Also:
 * https://tools.ietf.org/html/rfc4180
 */

#include <xtd.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <xtd/csv.h>
#include <xtd/estring.h>
#include <xtd/strparse.h>
#include <xtd/log.h>

static char csv_str_fmt[] = "%s";

#ifdef NDEBUG
#define debug_fields_(str)
#else
/*
 * debug_fields_() --Log field information at debug level.
 */
static void debug_fields_(const char *proc, int n, const CSVFieldPtr f)
{
    debug("%s: %d fields", proc, n);
    for (int i = 0; i < n; ++i)
    {
        debug("%s: field[%d]: %s", proc, i, f[i].item.name);
    }
}
#endif /* NDEBUG */

/*
 * csv_strip_eol_() --Strip common newline characters from a string.
 *
 * Parameters:
 * str  --specifies the string to be stripped
 */
static void csv_strip_eol_(char *str)
{
    char *end;

    if ((end = strchr(str, '\n')) != NULL)
    {                                  /* zap any newline characters */
        *end = '\0';
    }
    if ((end = strchr(str, '\r')) != NULL)
    {                                  /* hello windows! */
        *end = '\0';
    }
}

/*
 * csv_mk_header_() --Construct/malloc a header record from text.
 *
 * Parameters:
 * header   --specifies the header text
 * csv_fld  --returns the constructed list of header fields
 *
 * Returns: (size_t)
 * Success: the No. of fields in the header; Failure: 0.
 */
static size_t csv_mk_header_(char *header, CSVFieldPtr * csv_fld)
{
    char *h;
    size_t n;
    CSVFieldPtr field = NULL;
    static CSVField default_field = {
        {.type = STRING_TYPE},
        .scan_fmt = (char *) csv_str_fmt,
        .print_fmt = (char *) csv_str_fmt
    };

    if ((h = strdup(header)) == NULL)
    {
        return 0;                      /* error: strdup failed */
    }

    n = strsplit(h, ',');
    if ((field = NEW(CSVField, n)) == NULL)
    {
        free(h);
        return 0;                      /* error: calloc failed */
    }
    for (size_t i = 0; i < n; ++i)
    {
        field[i] = default_field;
        field[i].item.name = h;
        h += strlen(h) + 1;
    }
    csv_strip_eol_(field[n - 1].item.name);
    *csv_fld = field;

    debug_fields_(__func__, n, field); /* debug */
    return n;
}

/*
 * csv_cmp_header_() --Compare a file's header with a prepared header.
 *
 * Parameters:
 * header   --specifies the header text contained in the file
 * n_fld    --specifies the No. fields in the prepared header structure
 * csv_fld  --specifies the prepared header
 *
 * Returns: (int)
 * Success: 0 (headers are equivalent); Failure: 1 (headers differ).
 *
 * Remarks:
 * This routine only compares the names of the fields, not the types.
 */
static int csv_cmp_header_(char *header, size_t n_fld, CSVFieldPtr csv_fld)
{
    CSVFieldPtr field;
    size_t n_field;
    int diff_status = 0;

    if ((n_field = csv_mk_header_(header, &field)) == 0)
    {
        return !diff_status;           /* error: bad header in file? */
    }
    if (n_field == n_fld)
    {
        for (size_t i = 0; i < n_field; ++i)
        {                              /* compare every field name */
            if (strcmp(field[i].item.name, csv_fld[i].item.name) != 0)
            {
                diff_status = 1;       /* vive la difference! */
            }
        }
    }
    free(field);
    return !diff_status;
}

/*
 * csv_write_header_() --Write the CSV file header.
 *
 * Parameters:
 * csv_fp    --the CSV file pointer
 */
static void csv_write_header_(CSVFilePtr csv_fp)
{
    fputs(csv_fp->field[0].item.name, csv_fp->fp);

    for (size_t i = 1; i < csv_fp->n_field; ++i)
    {
        fputs(",", csv_fp->fp);
        fputs(csv_fp->field[i].item.name, csv_fp->fp);
    }
    fputs("\n", csv_fp->fp);
}

/*
 * csv_open() --Open a CSV file.
 *
 * Parameters:
 * path --specifies the path of the file to open
 * mode --specifies the file opening mode: one of "r", "w", "a"
 * ...  --other parameters, but only for "w", "a" modes
 *
 * Returns: (CSVFilePtr)
 * Success: the opened file; Failure: NULL.
 *
 * Remarks:
 * If the file is opened in "r" (readonly) mode, the header is
 * extracted from the file contents and a privately allocated header
 * control structure is automatically created (and subsequently freed
 * by csv_close()).  Otherwise, the caller must provide two extra parameters:
 *
 *  * n_fields  --the number of fields
 *  * fields    --an array of CSVField structures.
 *
 * In the case of "a" (append) mode, the file's header (if any) is parsed
 * and compared to the provided data, and it must match EXACTLY.  However,
 * the caller-provided header is used, and is assumed to be managed by
 * the caller (i.e. csv_close() will not free it).
 */
CSVFilePtr csv_open(const char *path, const char *mode, ...)
{
    va_list ap;
    CSVFilePtr csv_fp;
    char header[CSV_TEXT_MAX + 1] = "";

    if (!(*mode == 'a' || *mode == 'r' || *mode == 'w'))
    {
        return NULL;                   /* error: invalid mode */
    }

    info("%s: opening file \"%s\" (%s)", __func__, path, mode);
    if ((csv_fp = calloc(1, sizeof(CSVFile))) == NULL)
    {
        return NULL;                   /* error: malloc failed */
    }

    if ((csv_fp->fp = fopen(path, mode)) == NULL)
    {
        csv_close(csv_fp);
        trace_debug("cannot open file \"%s\"", path);
        return NULL;                   /* error: fopen failed */
    }

    csv_fp->mode = *mode;

    if (*mode != 'r')
    {
        va_start(ap, mode);
        csv_fp->n_field = (size_t) va_arg(ap, int);

        csv_fp->field = va_arg(ap, CSVFieldPtr);
        va_end(ap);

        trace_debug("%zu fields", csv_fp->n_field);
        if (*mode == 'a' && !feof(csv_fp->fp))
        {
            fgets(header, sizeof(header), csv_fp->fp);
            if (csv_cmp_header_(header, csv_fp->n_field, csv_fp->field) != 0)
            {
                csv_close(csv_fp);
                trace_debug("%s: inconsistent header \"%s\"", path, header);
                return NULL;           /* error: inconsistent header */
            }
        }
        else
        {                              /* dump the header now! */
            csv_write_header_(csv_fp);
        }
    }
    else
    {
        fgets(header, sizeof(header), csv_fp->fp);
        if ((csv_fp->n_field = csv_mk_header_(header, &csv_fp->field)) == 0)
        {
            csv_close(csv_fp);
            trace_debug("invalid header \"%s\"", path);
            return NULL;               /* error: bad header? */
        }
    }
    return csv_fp;
}

/*
 * csv_close() --Close a fully/partially open CSV file.
 *
 * Parameters:
 * csv_fp    --the CSV file pointer
 *
 * Remarks:
 * This routine is called when csv_open() bails, so the CSVFile may be
 * in an inconsistent state.  Note that if the file was opened in
 * readonly mode, the field information was allocated by this module,
 * so we free it too.  Otherwise, we assume that the caller allocated
 * and is managing the memory.
 */
void csv_close(CSVFilePtr csv_fp)
{
    if (csv_fp->fp != NULL)
    {
        fclose(csv_fp->fp);
    }
    if (csv_fp->field != NULL && csv_fp->mode == 'r')
    {
        free(csv_fp->field[0].item.name);
        free(csv_fp->field);
    }
    free(csv_fp);
}

/*
 * csv_read() --Read a record from the CSV file.
 *
 * Paramters:
 * csv_fp    --the CSV file pointer
 * n_value --specifies the number of values to read
 * values   --returns the values read
 * n_byte --specifies the maximum number of bytes to read
 * bytes   --a buffer to read the bytes into (size >= n_byte)
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This raw record is read by fgets into the bytes buffer, and from
 * there it is processed into the values array.  Note that (for string
 * values at least) values.item.value will address bytes[] storage.
 * This is OK, since it's all the callers data-space from csv_read's POV.
 *
 * However, for convenience we save a reference in the CSV's field
 * structure too, so that callers can use the field list directly.
 * This breaks every rule in the book relating to data design and
 * modularity, but, uh, sometimes cheezy is useful, ya know?
 */
int csv_read(CSVFilePtr csv_fp, size_t n_value, Atom value[],
             size_t n_byte, char bytes[])
{
    char *cp = bytes;
    size_t n_fields;
    CSVFieldPtr fld = csv_fp->field;
    AtomPtr val = value;

    if (csv_fp->mode != 'r')
    {
        return 0;
    }

    if (fgets(bytes, n_byte, csv_fp->fp) == NULL)
    {
        return 0;                      /* eof */
    }

    n_fields = strsplit(bytes, ',');
    n_value = MIN(n_value, n_fields);

    for (; n_value > 0; ++fld, ++val, --n_value)
    {

        if (fld->scan_fmt == csv_str_fmt)
        {
            val->value.string = cp;
            csv_strip_eol_(cp);
        }
        else
        {
            sscanf(cp, fld->scan_fmt, &val->value);
        }
        val->type = fld->item.type;
        fld->item.value = val->value;  /* remember last read value */
        cp += strlen(cp) + 1;
    }
    return 1;
}

/*
 * csv_write() --Write a record to the CSV file.
 *
 * Paramters:
 * csv_fp    --the CSV file pointer
 * n        --specifies the number of values to write
 * values   --specifies the values to write
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * The values are printed with the field's "print_fmt" specifier.
 * That's OK for now, but one day I'll need to deal with escaping
 * characters (e.g. "\n" etc.) and possibly quoting the whole field in
 * the way that excel does.
 */
int csv_write(CSVFilePtr csv_fp, size_t n_value, Atom value[])
{
    CSVFieldPtr fld = csv_fp->field;
    AtomPtr val = value;

    if (csv_fp->mode == 'r')
    {
        return 0;                      /* error: this mode can't write */
    }

    n_value = MIN(n_value, csv_fp->n_field) - 1;
    fprintf(csv_fp->fp, fld->print_fmt, val->value);

    for (++fld, ++val; n_value > 0; ++fld, ++val, --n_value)
    {
        fputs(",", csv_fp->fp);
        fprintf(csv_fp->fp, fld->print_fmt, val->value);
    }
    fputs("\n", csv_fp->fp);
    return 1;
}

/*
 * csv_field() --Find a CSV file's field by name.
 *
 * Parameters:
 * csv_fp    --the CSV file pointer
 * name     --the name of the field to search for
 *
 * Returns: (SymbolPtr)
 * Success: a pointer to the field; Failure: NULL.
 */
CSVFieldPtr csv_field(CSVFilePtr csv_fp, const char *name)
{
    CSVFieldPtr fld = csv_fp->field;
    int slot;

    if (str_int(name, &slot) && slot > 0
        && (unsigned) slot <= csv_fp->n_field)
    {                                  /* allow numeric columns too! */
        return csv_fp->field + slot - 1;
    }

    for (int n_field = csv_fp->n_field; n_field > 0; ++fld, --n_field)
    {
        if (strcmp(fld->item.name, name) == 0)
        {
            return fld;
        }
    }
    return NULL;
}

/*
 * parse_fields() --Parse a list of names into a vector of CSVFieldPtrs.
 *
 * Returns: (CSVFieldPtr *)
 * Success: a vector of pointers to CSVFieldPtrs; Failure: NULL.
 *
 * Remarks:
 * The returned vector is allocated on the heap; it is the caller's
 * responsibility to free it.
 */
CSVFieldPtr *csv_parse_fields(CSVFilePtr csv_fp, char *fields)
{
    CSVFieldPtr *field = new_vector(sizeof(*field), 0, NULL);
    int n = strsplit(fields, ',');
    const char *unk_field = "unknown field \"%s\"";

    while (n--)
    {
        CSVFieldPtr f1, f2;
        int len = strlen(fields);
        char *alt = strchr(fields, '-');

        if (alt)
        {                              /* handle "f1-f2" */
            *alt++ = '\0';
            if (STREMPTY(fields))
            {                          /* "-*" */
                f1 = csv_fp->field;
            }
            else
            {                          /* "x-*" */
                f1 = csv_field(csv_fp, fields);
            }
            if (STREMPTY(alt))
            {                          /* "*-" */
                f2 = csv_fp->field + csv_fp->n_field - 1;
            }
            else
            {                          /* "*-y" */
                f2 = csv_field(csv_fp, alt);
            }
        }
        else
        {                              /* find simple field */
            f1 = f2 = csv_field(csv_fp, fields);
        }

        if (f1 == NULL)
        {
            err(unk_field, fields);
            free_vector(field);
            field = NULL;
            break;
        }
        if (f2 == NULL)
        {
            err(unk_field, alt);
            free_vector(field);
            field = NULL;
            break;
        }
        if (f2 < f1)
        {
            err("invalid field specification \"%s-%s\"", fields, alt);
            free_vector(field);
            field = NULL;
            break;
        }
        for (CSVFieldPtr f = f1; f <= f2; ++f)
        {
            field = vector_add(field, 1, &f);
        }
        fields += len + 1;
    }
    return field;
}
