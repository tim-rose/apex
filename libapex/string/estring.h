/*
 * ESTRING.H --Definitions for some extended C-string functions.
 *
 */
#ifndef APEX_ESTRING_H
#define APEX_ESTRING_H
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include <apex.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef char tiny_string[256];
    typedef char medium_string[4096];

    extern const char empty_str[];

#define STR_OR_NULL(_str) (((_str) == NULL) ? "(null)" : (_str))
#define STREMPTY(str) ( str == NULL || *(str) == '\0')

    int strempty(const char *str);
    int strequiv(const char *s1, const char *s2);
    int strprefix(const char *str, const char *prefix);
    char *estrcpy(char *dst, const char *src);
    char *estrncpy(char *dst, const char *src, size_t n);
    char *estrcat(char *dst, size_t n, const char *str[]);
    char *vstrcat(char *dst, const char *str, ... /* NULL-terminated */ );
    char *estrjoin(char *dst, int delim, size_t n, const char *str[]);
    char *vstrjoin(char *dst, int delim, const char *str,
                   ... /* NULL-terminated */ );
    char *strtrunc(char *str, size_t len, char *text_buffer);
    char *estrtolower(char *str);
    char *estrtoupper(char *str);
    char *estrmap(char *dst, const char *src, int (*func)(int ch));
    char *estrsub(char *str, int match, int replace, bool global);
    char *estrtr(char *str, const char *match, const char *replace);
    size_t strsplit(char *str, int delimter);
    char **new_str_list(const char *str, int delimiter);
    void free_str_list(char **list);
    int vstrmatch(const char *target, const char *candidate, ...);
    int vstrcasematch(const char *target, const char *candidate, ...);
#ifdef NO_ASPRINTF
    int asprintf(char **str, const char *fmt, ...) PRINTF_ATTRIBUTE(2, 3);
    int vasprintf(char **str, const char *fmt, va_list args);
#endif                                 /* NO_ASPRINTF */
    void memswap(void *m1, void *m2, size_t n);
    void memswap_int(int *i1, int *i2, size_t n);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_ESTRING_H */
