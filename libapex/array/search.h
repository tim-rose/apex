/*
 * SEARCH.H --Array searching routines.
 *
 */
#ifndef SEARCH_H
#define SEARCH_H

#include <stdlib.h>
#include <stdbool.h>
#include <apex/compare.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    int binsearch(void *key, void *base, size_t n_elements, size_t size,
                  CompareProc compare, bool *status);

    int lsearch(void *key, void *base, size_t n_elements, size_t size, 
                  CompareProc compare, bool *status);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* SEARCH_H */
