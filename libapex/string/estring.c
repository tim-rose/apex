/*
 * ESTRING.C --some extended C-string functions.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * strempty()      --Test if a string is empty.
 * strequiv()      --Compare two strings, allowing for NULL pointers.
 * strprefix()     --Test if a string has a specified prefix.
 * estrcpy()       --Copy a string, returning a pointer to the end-of-copy.
 * estrncpy()      --Copy a string, returning a pointer to the end-of-copy.
 * estrcat()       --Concatentate an array of strings.
 * vstrcat()       --Concatentate a (NULL-terminated) list of strings.
 * estrjoin()      --Join an array of strings with a specified delimiter.
 * vstrjoin()      --Join a (NULL-terminated) list of strings with a delimiter.
 * vstrmatch()     --Match a string against a list of candidates.
 * vstrcasematch() --Match a string against a list of candidates (case insensitive)
 * asprintf()      --Duplicate a printf-formatted string.
 * vasprintf()     --Duplicate a formatted string (stdarg version).
 *
 * Remarks:
 * This module provides some additional operations on ASCIZ strings.
 * One of the chief differences over the Unix string routines is that
 * these routines return the *end* of the string operated on, rather
 * than the start, where possible.
 *
 * References:
 * string(3c).
 */
#include <apex.h>                       /* Windows_NT requires this before system headers */

#include <limits.h>
#include <stdio.h>
#include <ctype.h>

#include <apex/estring.h>

const char empty_str[] = "";

/*
 * strempty() --Test if a string is empty.
 *
 * Parameters:
 * str  --the string to be tested
 *
 * Returns: (int)
 * 1: the string is empty; 0: the string is not empty.
 */
int strempty(const char *str)
{
    return STREMPTY(str);
}

/*
 * strequiv() --Compare two strings, allowing for NULL pointers.
 */
int strequiv(const char *s1, const char *s2)
{
    if (s1 != NULL && s2 != NULL)
    {
        return strcmp(s1, s2);
    }
    return s1 != s2;                   /* NULL == NULL */
}

/*
 * strprefix() --Test if a string has a specified prefix.
 */
int strprefix(const char *str, const char *prefix)
{
    while (*prefix != '\0' && *prefix == *str)
    {
        ++prefix;
        ++str;
    }
    return *prefix == '\0';
}

/*
 * estrcpy() --Copy a string, returning a pointer to the end-of-copy.
 *
 * Parameters:
 * dst  --the destination string buffer
 * src  --the string to be copied
 *
 * Returns: (char *)
 * The end of the string copy (i.e. dst+strlen(src)).
 */
char *estrcpy(char *dst, const char *src)
{
    while (*src != '\0')
    {
        *dst++ = *src++;
    }
    *dst = '\0';
    return dst;
}

/*
 * estrncpy() --Copy a string, returning a pointer to the end-of-copy.
 *
 * Parameters:
 * dst  --the destination string buffer
 * src  --the string to be copied
 * n    --the maximum No. of characters to copy
 *
 * Returns: (char *)
 * The end of the string copy.
 *
 * Remarks:
 * Note that (as with strncpy(3c)) dst is not guaranteed to be
 * null-terminated, however the return value will be "right" in either
 * case (i.e. pointing to the '\0', or where the '\0' should have been
 * if there was room for it.
 */
char *estrncpy(char *dst, const char *src, size_t len)
{
    int n = len;

    while (n-- > 0 && (*(dst++) = *(src++)) != '\0')
        ;
    return (n <= 0) ? dst : dst - 1;
}

/*
 * estrcat() --Concatentate an array of strings.
 *
 * Parameters:
 * dst      --the destination string buffer
 * n        --the number of strings
 * src      --the strings to be copied
 *
 * Returns: (char *)
 * The end of the copied strings (i.e. dst+strlen(src[0])+...).
 */
char *estrcat(char *dst, size_t n, const char *src[])
{
    size_t i;

    for (i = 0; i < n; ++i)
    {
        dst = estrcpy(dst, src[i]);
    }
    return dst;
}

/*
 * vstrcat() --Concatentate a (NULL-terminated) list of strings.
 *
 * Parameters:
 * dst      --the destination string buffer
 * src,...  --the strings to be copied
 *
 * Returns: (char *)
 * The end of the copied strings (i.e. dst+strlen(src1)+...).
 */
char *vstrcat(char *dst, const char *src, ... /* NULL */ )
{
    va_list args;

    va_start(args, src);

    if (src != NULL)
    {
        dst = estrcpy(dst, src);
        while ((src = va_arg(args, char *)) != NULL)
        {
            dst = estrcpy(dst, src);
        }
    }
    va_end(args);
    return dst;
}

/*
 * estrjoin() --Join an array of strings with a specified delimiter.
 *
 * Parameters:
 * dst      --the destination string buffer
 * delim    --the delimiter character
 * n        --the number of strings
 * src,...  --the strings to be joined
 *
 * Returns: (char *)
 * The end of the copied strings.
 */
char *estrjoin(char *dst, int delim, size_t n, const char *src[])
{
    size_t i = 0;

    if (n > 0)
    {
        dst = estrcpy(dst, src[i]);
        for (i = 1; i < n; ++i)
        {
            *dst++ = delim;
            dst = estrcpy(dst, src[i]);
        }
    }

    return dst;
}

/*
 * vstrjoin() --Join a (NULL-terminated) list of strings with a delimiter.
 *
 * Parameters:
 * dst      --the destination string buffer
 * delim    --the delimiter
 * src,...  --the strings to be copied
 *
 * Returns: (char *)
 * The end of the copied strings.
 */
char *vstrjoin(char *dst, int delim, const char *src, ... /* NULL */ )
{
    va_list args;

    va_start(args, src);

    if (src != NULL)
    {
        dst = estrcpy(dst, src);
        while ((src = va_arg(args, char *)) != NULL)
        {
            *dst++ = delim;
            dst = estrcpy(dst, src);
        }
    }
    va_end(args);
    return dst;
}

/*
 * vstrmatch() --Match a string against a list of candidates.
 *
 * Parameters:
 * target   --specifies the target to match
 * candidate... --specifies a NULL-terminated list of candidate strings
 *
 * Returns: (int)
 * The slot that matched; Failure: -1;
 */
int vstrmatch(const char *target, const char *candidate, ...)
{
    va_list args;
    int match = -1;

    va_start(args, candidate);
    for (int idx = 0;
         candidate != NULL; candidate = va_arg(args, char *), ++idx)
    {
        if (strcmp(target, candidate) == 0)
        {
            match = idx;
            break;
        }
    }
    va_end(args);
    return match;
}

/*
 * vstrcasematch() --Match a string against a list of candidates (case insensitive)
 *
 * Parameters:
 * target   --specifies the target to match
 * candidate... --specifies a NULL-terminated list of candidate strings
 *
 * Returns: (int)
 * The slot that matched; Failure: -1;
 */
int vstrcasematch(const char *target, const char *candidate, ...)
{
    va_list args;
    int match = -1;

    va_start(args, candidate);
    for (int slot = 0;
         candidate != NULL; candidate = va_arg(args, char *), ++slot)
    {
        if (strcasecmp(target, candidate) == 0)
        {
            match = slot;
            break;
        }
    }
    va_end(args);
    return match;
}

#ifdef NO_ASPRINTF
/*
 * asprintf() --Duplicate a printf-formatted string.
 *
 * Parameters:
 * str  --(address of) the copy destination string
 * fmt  --a printf-style string format
 * ...  --printf arguments for this format
 *
 * Returns: (char *)
 * Success: the duplicated formatted string; Failure: NULL.
 *
 * Remarks:
 * This function is (currently) present in BSD systems but not Linux...
 */
int asprintf(char **str, const char *fmt, ...)
{
    va_list args;
    int n;

    va_start(args, fmt);
    n = vasprintf(str, fmt, args);
    va_end(args);

    return n;
}

/*
 * vasprintf() --Duplicate a formatted string (stdarg version).
 *
 * Parameters:
 * str  --(address of) the copy destination string
 * fmt  --a printf-style string format
 * args --a stdarg va_list
 *
 * Returns: (char *)
 * Success: length of the string; Failure: -1.
 */
int vasprintf(char **str, const char *fmt, va_list args)
{
    size_t size = vsnprintf(NULL, 0, fmt, args) + 2;

    if ((*str = malloc(size)) != NULL)
    {
        return vsprintf(*str, fmt, args);
    }
    return -1;
}
#endif /* HAVE_ASPRINTF */
