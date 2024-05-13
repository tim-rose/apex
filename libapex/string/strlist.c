/*
 * STRLIST.C --Some routines for handling lists of strings.
 *
 * Contents:
 * strsplit()      --Split a string into components based on a delimiter.
 * new_str_list()  --Return a list of strings by splitting a string.
 * free_str_list() --Free the resources for a string list.
 */
#include <apex/estring.h>

/*
 * strsplit() --Split a string into components based on a delimiter.
 *
 * Parameters:
 * str  --the string to split
 * delimiter --the delimiter character
 *
 * Returns: (size_t)
 * Success: The number of strings created; Failure: 0.
 */
size_t strsplit(char *str, int delimiter)
{
    size_t n;
    char *end;

    if (str == NULL || delimiter == '\0')
    {
        return 0;                      /* error: bad args */
    }
    for (n = 1; (end = strchr(str, delimiter)); ++n, str = end)
    {
        *end++ = '\0';                 /* terminate str and skip null */
    }
    return n;                          /* return No. substrings */
}

/*
 * new_str_list() --Return a list of strings by splitting a string.
 *
 * Returns: (char **)
 * Success: the string list; Failure: 0.
 *
 * Remarks:
 * This routine returns a malloc'd copy, which must be release via
 * free_str_list.  The original string is not modified.
 */
char **new_str_list(const char *str, int delimiter)
{
    size_t n;
    char **list;
    char *s;

    if (str == NULL || delimiter == '\0')
    {
        return NULL;                   /* error: bad args */
    }

    if ((s = strdup(str)) == NULL)
    {
        return NULL;                   /* error: strdup failed */
    }
    n = strsplit(s, delimiter);
    if ((list = NEW(char *, n + 1)) == NULL)
    {
        free((void *) s);
        return NULL;                   /* error: malloc failed */
    }
    for (size_t i = 0; i < n; ++i)
    {
        list[i] = s;
        s += strlen(s) + 1;
    }                                  /* note: last item in list is naturally NULL */
    return list;                       /* success */
}

/*
 * free_str_list() --Free the resources for a string list.
 */
void free_str_list(char **list)
{
    free(list[0]);
    free(list);
}
