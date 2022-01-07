/*
 * ARRAY.H --A simple array array, with caller provided item storage.
 *
 * Remarks:
 * This is used as a base structure/class for other array-based data
 * structures (stack, heap, queue etc.).
 */
#ifndef ARRAY_H
#define ARRAY_H

#include <xtd.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct ArrayContainer_t
    {
        size_t n_items;
        size_t item_size;
        char *items;                   /* size: n_items*item_size */
    } ArrayContainer, *ArrayContainerPtr;
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* ARRAY_H */
