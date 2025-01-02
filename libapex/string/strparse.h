/*
 * STRPARSE.H -- Routines for parsing stuff from strings.
 *
 */
#ifndef APEX_STRPARSE_H
#define APEX_STRPARSE_H

#include <unistd.h>
#include <stdint.h>
#include <getopt.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    int str_inet4_address(const char *text, uint32_t * address,
                          uint32_t * network);
    int str_int(const char *text, int *result);
    int str_uint(const char *text, unsigned int *result);
    int str_uint16(const char *text, uint16_t * result);
    int str_int16(const char *text, int16_t * result);
    int str_double(const char *text, double *result);
    int str_float(const char *text, float *result);
    int str_int_list(const char *text, size_t n_items, int item[]);
    int str_str_list(char *text, size_t n_items, char *item[]);
    int str_int_in_range(const char *text, int *result, int min_value,
                         int max_value);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_STRPARSE_H */
