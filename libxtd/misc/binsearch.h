/*
 * BINSEARCH.H --Definitions for binsearch(), an improved version of bsearch().
 *
 */
#ifndef BINSEARCH_H
#define BINSEARCH_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    /*
     * CompareProc --Comparison function for ordered items.
     */
    typedef int (*CompareProc) (const void *data, const void *key);

    size_t binsearch(void *key, void *base, size_t n_elements, size_t size,
                     CompareProc compare, bool * status);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* BINSEARCH_H */
