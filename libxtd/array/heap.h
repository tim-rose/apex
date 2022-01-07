/*
 * HEAP.H --An implicit (array) heap.
 */
#ifndef HEAP_H
#define HEAP_H

#include <array.h>
#include <binsearch.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct Heap_t
    {
        ArrayContainer array;
        size_t n_used;
        CompareProc cmp;
    } Heap, *HeapPtr;

    HeapPtr heap_alloc(void);
    HeapPtr heap_init(HeapPtr heap, CompareProc cmp, size_t n_items,
                      size_t item_size, void *items);

    int heap_push(HeapPtr heap, const void *item);
    int heap_pop(HeapPtr heap, void *item);
    void *heap_peek(HeapPtr heap, void *item);

#define new_heap(cmp, items) init_heap(heap_alloc(), cmp, items)
#define init_heap(heap, cmp, items) heap_init(heap, cmp, NEL(items), sizeof(items[0]), items)
    /*
     * low-level heap operations.
     */
    int heap_ok(void *heap, size_t n_items, size_t item_size,
                CompareProc cmp);
    void heap_sift_up(void *heap, size_t n_items, size_t item_size,
                      CompareProc cmp);
    void heap_sift_down(void *heap, size_t n_items, size_t item_size,
                        CompareProc cmp);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* HEAP_H */
