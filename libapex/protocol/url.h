/*
 * URL.H --Definitions for parsing/processing URLs.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Remarks:
 * I'm sure someone's already done this, but I couldn't find a good
 * example anywhere.
 *
 */
#ifndef APEX_URL_H
#define APEX_URL_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct URL
    {
        char *scheme;
        char *user;
        char *password;
        char *domain;
        int port;
        char *path;
        char *query;
        char *anchor;
    } URL;

#define NULL_URL { .scheme = 0 }

    extern URL null_url;

    int getopt_url(char *opt, URL *url);
    int snprint_url(char *str, size_t n, URL *url);
    size_t url_encode(const char *text, size_t n, char *buf);
    int str_url(char *opt, URL *url);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_URL_H */
