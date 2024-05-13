/*
 * ESTRING.C --some extended C-string functions.
 *
 * Contents:
 * strtrunc()    --Truncate a string.
 * estrtolower() --Convert a string to lower-case, in-place.
 * estrtoupper() --Convert a string to upper-case, in-place.
 * estrsub()     --Substitute one character with another in a string, in-place.
 * estrtr()      --Perform an in-place character-set transformation on a string.
 * estrmap()     --Apply a function to every character in a string.
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
#include <ctype.h>
#include <apex/estring.h>

/*
 * strtrunc() --Truncate a string.
 *
 * Parameters:
 * str  --the string to be truncated.
 * len  --the maximum length of the string (excluding the terminating '\0')
 * buf  --a buffer to place the truncated string (or NULL)
 *
 * Returns: (char *)
 * Success: a possibly truncated copy of str; Failure: NULL.
 *
 * Remarks:
 * estrtrunc() will copy at most len characters into a buffer, and, if
 * the provided string is longer than len it overwrites the last part
 * of the copy with "...".
 *
 * If buf is NULL, a private buffer of known length is used.
 */
char *strtrunc(char *str, size_t len, char *text_buffer)
{
    static char private_buffer[100];
    static char ellipsis[] = "...";
    char *bp;
    size_t i;

    if (text_buffer == NULL)
    {                                  /* text_buffer not supplied... */
        text_buffer = private_buffer;  /* ...use one we prepared earlier! */
        len = MIN(len, sizeof(private_buffer) - 1);
    }
    bp = text_buffer;

    for (i = 0; i <= len; ++i)
    {
        if ((*bp++ = *str++) == '\0')
        {
            return text_buffer;        /* no need to truncate */
        }
    }
    if (len >= sizeof(ellipsis))
    {
        (void) estrcpy(bp - sizeof(ellipsis), ellipsis);
    }
    else
    {
        *--bp = '\0';
    }
    return text_buffer;                /* truncated + ellipsis */
}

/*
 * estrtolower() --Convert a string to lower-case, in-place.
 *
 * Parameters:
 * str  --specifies the string to be converted
 *
 * Returns: (char *)
 * The end of the string.
 *
 * Remarks:
 * This routine uses tolower(3c) to ensure locale-dependent behaviour.
 */
char *estrtolower(char *str)
{
    int c;

    for (; (c = *str) != '\0'; ++str)
    {
        *str = tolower(c);
    }
    return str;
}

/*
 * estrtoupper() --Convert a string to upper-case, in-place.
 *
 * Parameters:
 * str  --specifies the string to be converted
 *
 * Returns: (char *)
 * The end of the string.
 *
 * Remarks:
 * This routine uses toupper(3c) to ensure locale-dependent behaviour.
 */
char *estrtoupper(char *str)
{
    int c;

    for (; (c = *str) != '\0'; ++str)
    {
        *str = toupper(c);
    }
    return str;
}

/*
 * estrsub() --Substitute one character with another in a string, in-place.
 *
 * Parameters:
 * str  --the string to apply the substitution
 * match    --the character to replace
 * replace  --the character to replace it with
 * all   --specifies whether to substitute globally (i.e. all occurrances)
 *
 * Returns: (char *)
 * The end of str.
 */
char *estrsub(char *str, int match, int replace, bool all)
{
    char *chptr;

    for (chptr = strchr(str, match); chptr != NULL;
         chptr = strchr(str, match))
    {
        *chptr = replace;
        if (!all)
        {
            break;                     /* once is enough */
        }
        str = chptr + 1;
    }
    return (str + strlen(str));        /* end of string */
}

/*
 * estrtr() --Perform an in-place character-set transformation on a string.
 *
 * Parameters:
 * str  --the string to be transformed
 *
 * Returns: (char *)
 * The end of the string.
 *
 * Remarks:
 * This routine isn't very efficient (yet), because it scans the
 * match set for every character.  A better scheme would build
 * a translation table from match+replace, and use that to direct
 * the transformation.
 * TODO: make estrtr() work with a translation table.
 */
char *estrtr(char *str, const char *match, const char *replace)
{
    int c;

    for (c = *str; c != '\0'; c = *str)
    {
        char *chptr;

        if ((chptr = strchr(match, c)) != NULL)
        {
            *str = replace[chptr - match];
        }
        ++str;
    }
    return str;
}


/*
 * estrmap() --Apply a function to every character in a string.
 *
 * Parameters:
 * dst  --specifies the output string
 * str  --specifies the input string
 * func --specifies the function to call
 *
 * Returns: (char *)
 * The end of the output string.
 */
char *estrmap(char *dst, const char *src, int (*func)(int c))
{
    int c;

    for (; (c = *src) != '\0'; ++src)
    {
        *dst++ = func(c);
    }
    return dst;
}
