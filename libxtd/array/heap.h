/*
 * HEAP.H --Definitions for simple heap.
 *
 */
#ifndef HEAP_H
#define HEAP_H

#include <binsearch.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    void heap_sift_up(void *base, size_t n_items, size_t item_size, CompareProc cmp);
    void heap_sift_down(void *base, size_t n_items, size_t item_size,
                        CompareProc cmp);
    int heap_ok(void *base, size_t n_items, size_t item_size, CompareProc cmp);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* HEAP_H */
