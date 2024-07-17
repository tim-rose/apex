/*
 * STRPARSE.C --Simple string parsing utilities.
 *
 * Contents:
 * str_inet4_address() --Parse a "host[/bits]" inet4 address.
 * str_int()           --Parse a number, as a natural int value.
 * str_uint()          --Parse a number, as a natural unsigned int value.
 * str_uint16()        --Parse a number, as an unsigned 16-bit value.
 * str_int16()         --Parse a number as a 16-bit value.
 * str_double()        --Parse a number, as a double-precision float value.
 * str_float()         --Parse a number, as a float value.
 * parse_int_range()   --Parse an integer range of the form "a-b".
 * str_int_list()      --Parse a list of integers.
 * str_str_list()      --Parse a list of strings.
 * str_int_in_range()  --Parse an int, and validate its value wrt boundaries.
 *
 */
#include <apex.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>

#include <apex/strparse.h>
#include <apex/estring.h>

/*
 * str_inet4_address() --Parse a "host[/bits]" inet4 address.
 *
 * Parameters:
 * text  --text containing the address to be parsed
 * address  --returns the inet4 address as a uint32_t
 * netmask  --returns the network mask, if not NULL.
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
#ifdef __WINNT__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif /* __WINNT__ */
int str_inet4_address(const char *text, uint32_t * address,
                      uint32_t * netmask)
{
    struct in_addr addr;
    char host[FILENAME_MAX];
    char mask[FILENAME_MAX];
    int mask_bits = 0;

    if (text == NULL)
    {
        return 0;                      /* error: nothing specified */
    }

    if (sscanf(text, "%[^/]/%s", host, mask) == 2)
    {
        text = host;
    }
    if (inet_pton(AF_INET, text, &addr))
    {
        *address = ntohl(addr.s_addr);
    }
    else
    {
        return 0;                      /* error: failed to parse/lookup */
    }
    if (netmask != NULL && *mask != '\0')
    {
        if (str_int(mask, &mask_bits))
        {
            mask_bits = 32 - mask_bits;
            *netmask = (0xffffffff >> mask_bits) << mask_bits;
        }
        else
        {
            if (inet_pton(AF_INET, mask, &addr))
            {
                *netmask = ntohl(addr.s_addr);
            }
        }
    }
    return 1;
}

/*
 * str_int() --Parse a number, as a natural int value.
 *
 * Parameters:
 * text  --the text containing a parseable value
 * result --returns the parsed value, if it succeeds
 *
 * Return: (int)
 * Success: 1; Failure: 0.
 */
int str_int(const char *text, int *result)
{
    int value;
    char *end;

    if (text == NULL)
    {
        return 0;
    }
    errno = 0;                         /* reset errno! */
    value = strtol(text, &end, 0);
    if (errno == 0 && *end == '\0')
    {
        *result = value;
        return 1;
    }
    return 0;
}

/*
 * str_uint() --Parse a number, as a natural unsigned int value.
 *
 * Parameters:
 * text  --the text containing a parseable value
 * result --returns the parsed value, if it succeeds
 *
 * Return: (int)
 * Success: 1; Failure: 0.
 */
int str_uint(const char *text, unsigned int *result)
{
    unsigned int value;
    char *end;

    if (text == NULL)
    {
        return 0;
    }
    errno = 0;                         /* reset errno! */
    value = strtoul(text, &end, 0);
    if (errno == 0 && *end == '\0')
    {
        *result = value;
        return 1;
    }
    return 0;
}

/*
 * str_uint16() --Parse a number, as an unsigned 16-bit value.
 *
 * Parameters:
 * text  --the text containing a parseable value
 * result --returns the parsed value, if it succeeds
 *
 * Return: (int)
 * Success: 1; Failure: 0.
 */
int str_uint16(const char *text, uint16_t * result)
{
    int value;
    int status;

    if ((status = str_int(text, &value)))
    {
        *result = value;
    }
    return status;
}

/*
 * str_int16() --Parse a number as a 16-bit value.
 *
 * Parameters:
 * text  --the text containing a parseable value
 * result --returns the parsed value, if it succeeds
 *
 * Return: (int)
 * Success: 1; Failure: 0.
 */
int str_int16(const char *text, int16_t * result)
{
    int value;
    int status;

    if ((status = str_int(text, &value)))
    {
        *result = value;
    }
    return status;
}

/*
 * str_double() --Parse a number, as a double-precision float value.
 *
 * Parameters:
 * text  --the text containing a parseable value
 * result --returns the parsed value, if it succeeds
 *
 * Return: (int)
 * Success: 1; Failure: 0.
 */
int str_double(const char *text, double *result)
{
    double value;
    char *end;

    if (text == NULL)
    {
        return 0;
    }
    errno = 0;                         /* reset errno! */
    value = strtof(text, &end);
    if (errno == 0 && *end == '\0')
    {
        *result = value;
        return 1;
    }
    return 0;
}

/*
 * str_float() --Parse a number, as a float value.
 *
 * Parameters:
 * text  --the text containing a parseable value
 * result --returns the parsed value, if it succeeds
 *
 * Return: (int)
 * Success: 1; Failure: 0.
 */
int str_float(const char *text, float *result)
{
    double value;
    int status;

    if ((status = str_double(text, &value)))
    {
        *result = value;
    }
    return status;
}

/*
 * parse_int_range() --Parse an integer range of the form "a-b".
 */
static int parse_int_range(const char *str, int *l, int *h, char **end_ptr)
{
    char *end;

    errno = 0;
    *l = strtol(str, &end, 0);
    if (errno != 0)
    {
        return 0;                      /* error: failed to parse */
    }
    if (*end != '-')
    {
        *h = *l;                       /* trivial range */
    }
    else
    {                                  /* handle range */
        str = end + 1;                 /* step past "-" */
        *h = strtol(str, &end, 0);
        if (errno != 0)
        {
            return 0;                  /* error: failed to parse */
        }
        if (*h < *l)
        {
            return 0;                  /* error: bad range */
        }
    }
    if (end_ptr)
    {
        *end_ptr = end;
    }
    return 1;
}

/*
 * str_int_list() --Parse a list of integers.
 *
 * Parameters:
 * text      --the string specifying a list of integers
 * n_items  --specifies the size of the item buffer
 * item     --returns the parsed values
 *
 * Returns: (int)
 * Success: the number of items parsed; Failure: 0.
 *
 * Remarks:
 * This routine accepts lists specified as a comma-separated
 * collection of ranges, similar to the field-list format
 * supported by cut(1).
 */
int str_int_list(const char *text, size_t n_items, int item[])
{
    size_t i = 0;
    int low, high;
    char *end;

    while (i < n_items)
    {
        if (!parse_int_range(text, &low, &high, &end))
        {
            return 0;                  /* failure: bad range of some sort */
        }
        while (i < n_items && low <= high)
        {
            item[i++] = low++;
        }
        if (low <= high)
        {
            return 0;                  /* error: array overflow */
        }
        if (STREMPTY(end))
        {
            return i;                  /* success: end of string */
        }
        if (*end != ',')
        {
            return 0;                  /* failure: not comma delimited */
        }
        text = end + 1;                /* step past "," */
    }
    return 0;                          /* error: array overflow */
}

/*
 * str_str_list() --Parse a list of strings.
 *
 * Parameters:
 * text      --the string specifying a list of integers
 * n_items  --specifies the size of the item buffer
 * item     --returns the parsed values
 *
 * Returns: (int)
 * Success: the number of items parsed; Failure: 0.
 *
 * Remarks:
 * This routine accepts lists specified as a comma-separated or
 * space-separated strings.  The text string is modified in-place to
 * add string terminating NULs, and item[] will contain the pointers
 * into it.
 */
int str_str_list(char *text, size_t n_items, char *item[])
{
    size_t i = 0;
    char *end;

    for (;;)
    {
        while (*text == ' ')
        {                              /* skip/blast leading spaces */
            *text++ = '\0';
        }
        if (STREMPTY(text))
        {
            return i;                  /* success: no text remains */
        }
        if (i >= n_items)
        {
            break;                     /* error: no room for any more */
        }
        item[i++] = text;
        if ((end = strchr(text, ',')) != NULL)
        {
            *end++ = '\0';
        }
        else
        {
            if ((end = strchr(text, ' ')) == NULL)
            {
                return i;              /* success: no delimiters remain */
            }
        }
        text = end;
    }
    return 0;                          /* error: array overflow */
}


/*
 * str_int_in_range() --Parse an int, and validate its value wrt boundaries.
 */
int str_int_in_range(const char *text, int *result, int min_value,
                     int max_value)
{
    int value;

    if (str_int(text, &value))
    {                                  /* parse OK */
        if (value >= min_value && value <= max_value)
        {
            *result = value;
            return 1;                  /* success: value in range */
        }
    }
    return 0;                          /* failure: unparsable or out of range */
}
