/*
 * URL.H --Definitions for parsing/processing URLs.
 *
 * Remarks:
 * I'm sure someone's already done this, but I couldn't find a good
 * example anywhere.
 *
 */
#ifndef URL_H
#define URL_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct URL_t
    {
        char *scheme;
        char *user;
        char *password;
        char *domain;
        int port;
        char *path;
        char *query;
        char *anchor;
    } URL, *URLPtr;

#define NULL_URL { .scheme = 0 }

    extern URL null_url;

    int getopt_url(char *opt, URLPtr url);
    int snprint_url(char *str, size_t n, URLPtr url);
    size_t url_encode(const char *text, size_t n, char *buf);
    int str_url(char *opt, URLPtr url);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* URL_H */
