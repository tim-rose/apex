/*
 * BINSEARCH.H --binsearch(), an improved version of bsearch().
 *
 */
#ifndef BINSEARCH_H
#define BINSEARCH_H

#include <stdlib.h>
#include <stdbool.h>
#include <xtd/compare.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    int binsearch(void *key, void *base, int n_elements, int size,
                  CompareProc compare, bool *status);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* BINSEARCH_H */
