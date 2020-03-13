/*
 * NMEA.C --Parsing/Processing routines for NMEA messages.
 *
 * Contents:
 * nmea_checksum() --Return the nmea_checksum for some text.
 * nmea_parse()    --Parse an NMEA message specified as a string.
 * nmea_fget()     --Read a line from a file, and parse it as an NMEA message.
 * nmea_fmt()      --Format an NMEA message to the wire protocol.
 * nmea_fputs()    --Print a NMEA message to a file.
 *
 * Remarks:
 * This module provides processing routines for NMEA-0183 format messages.
 * NMEA is commonly used by GPS equipment.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <log.h>
#include <estring.h>
#include <nmea.h>

/*
 * nmea_checksum() --Return the nmea_checksum for some text.
 */
unsigned int nmea_checksum(const char *str, const char *end)
{
    unsigned long check = 0;

    while (str < end)
    {                                  /* calculate checksum from data */
        check ^= (unsigned long) *str++;    /* simple XOR */
    }
    return check;
}

/*
 * nmea_parse() --Parse an NMEA message specified as a string.
 *
 * Parameters:
 * nmea_buf    --returns the parsed NMEA message
 * str      --specifies the NMEA message string
 *
 * Returns: (int)
 * Success: NMEA_OK; Failure: NEMA_EOF, NMEA_ERR.
 */
int nmea_parse(NmeaPtr nmea, const char *str)
{
    char *ptr, *end;
    unsigned check;
    size_t i;

    if (str == NULL || *str++ != '$' || (end = strchr(str, '\n')) == NULL)
    {
        return NMEA_ERR;               /* error: missing prologue */
    }
    if (*(end - 1) == '\r')
    {
        --end;                         /* skip DOS <cr> */
    }
    if (*(end - 3) == '*')
    {                                  /* read checksum from record */
        end -= 3;
        check = strtoul(end + 1, NULL, 16);
        if (check != nmea_checksum(str, end))
        {
            debug("nmea_parse: checksum expected %x, got %x",
                  nmea_checksum(str, end), check);
            return NMEA_ERR;           /* error: checksum failure */
        }
    }

    if (end - str > NMEA_LINE_MAX)
    {
        return NMEA_ERR;               /* error: payload too big */
    }

    *(estrncpy(nmea->id, str, sizeof(nmea->id) - 1)) = '\0';
    str += sizeof(nmea->id) - 1;

    *(estrncpy(nmea->msg, str, sizeof(nmea->msg) - 1)) = '\0';
    str += sizeof(nmea->msg) - 1;

    estrncpy(nmea->text, str, (size_t) (end - str));    /* take mung-able copy */
    nmea->text[end - str] = '\0';

    nmea->n_values = strsplit(nmea->text, ',');
    for (ptr = nmea->text, i = 0; i < nmea->n_values; ++i)
    {                                  /* gather all the pieces... */
        nmea->value[i] = ptr;
        ptr += strlen(ptr) + 1;
    }

    return NMEA_OK;                    /* success */
}

/*
 * nmea_fget() --Read a line from a file, and parse it as an NMEA message.
 *
 * Parameters:
 * nmea    --returns the parsed NMEA message
 * fp       --file to read line from
 *
 * Returns: (int)
 * Success: NMEA_OK; Failure: NEMA_EOF, NMEA_ERR.
 */
int nmea_fget(NmeaPtr nmea, FILE * fp)
{
    char text[NMEA_LINE_MAX + 3];

    if (fgets(text, sizeof(text), fp) == NULL)
    {
        if (feof(fp))
        {
            return NMEA_EOF;           /* error: end of file */
        }
        return NMEA_ERR;               /* error: fgets()-related */
    }
    return nmea_parse(nmea, text);
}

/*
 * nmea_fmt() --Format an NMEA message to the wire protocol.
 *
 * Parameters:
 * nmea --the nmea message to format
 * str  --specifies a text buffer, returns the formatted message
 *
 * Returns: (char*)
 * The NMEA formatted message.
 *
 * Remarks:
 * This routine does not check for buffer overflow!
 */
char *nmea_fmt(NmeaPtr nmea, char *str)
{
    char *start = str;
    unsigned int checksum;
    char check[3] = { 0 };
    char hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    str = vstrcat(str, "$", nmea->id, nmea->msg, NULL);
    str = estrjoin(str, ',', nmea->n_values, (const char **) nmea->value);

    checksum = nmea_checksum(start + 1, str);
    check[0] = hex[(checksum >> 4) & 0x0f];
    check[1] = hex[checksum & 0x0f];

    vstrcat(str, "*", check, "\r\n", NULL);

    return start;
}

/*
 * nmea_fputs() --Print a NMEA message to a file.
 *
 * Parameters:
 * nmea --the nmea message to format
 * fp   --the file pointer (open for writing)
 *
 * Returns: (int)
 * Propagated fputs() return value.
 */
int nmea_fputs(NmeaPtr nmea, FILE * fp)
{
    char text[NMEA_LINE_MAX + 3];

    return fputs(nmea_fmt(nmea, text), fp);
}
