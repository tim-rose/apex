/*
 * COMPARE.H --Definitions for compare functions.
 *
 * Remarks:
 * These are used for qsort, bsearch, etc.
 */
#ifndef COMPARE_H
#define COMPARE_H

#include <stdlib.h>
#include <stdbool.h>
#include <compare.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    /*
     * CompareProc --Comparison function for ordered items.
     */
    typedef int (*CompareProc)(const void *data, const void *key);

    int int_cmp(const void *v_1, const void *v_2);
    int long_cmp(const void *v_1, const void *v_2);
    int float_cmp(const void *v_1, const void *v_2);
    int double_cmp(const void *v_1, const void *v_2);
    int strp_cmp(const void *v_1, const void *v_2);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* COMPARE_H */
