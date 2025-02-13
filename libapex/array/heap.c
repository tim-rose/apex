/*
 * HEAP.C --A simple heap array, with caller provided item storage.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * heap_alloc()  --Allocate some space for a heap structure.
 * heap_init()   --Initialise a heap structure.
 * heap_push()   --Insert an item into the heap.
 * heap_pop()    --Remove an item from the top of the heap.
 * heap_peek()   --Peek at the top item in the heap.
 * heap_delete() --Delete an item from the heap.
 */
#include <memory.h>
#include <apex/heap.h>

/*
 * heap_alloc() --Allocate some space for a heap structure.
 *
 * Returns: (Heap *)
 * Success: the allocated memory; Failure: NULL.
 */
/* LCOV_EXCL_START */
Heap *heap_alloc(void)
{
    return malloc(sizeof(Heap));
}

/* LCOV_EXCL_STOP */

/*
 * heap_init() --Initialise a heap structure.
 *
 * Parameters:
 * heap --specifies and returns the initialised heap
 * cmp --item comparison function
 * n_items --the number of heap items
 * item_size --the size of each item
 * base --the storage for the heap (n_items*item_size)
 *
 * Returns: (Heap *)
 * Success: The heap; Failure: NULL.
 *
 * Remarks:
 * The heap storage is not allocated by this module, it must be provided
 * by the caller.
 */
Heap *heap_init(Heap *heap, CompareProc cmp, size_t n_items,
                  size_t item_size, void *base)
{
    if (heap != NULL && base != NULL)
    {
        array_init(&heap->array, n_items, item_size, base);
        heap->n_used = 0;
        heap->cmp = cmp;
        return heap;
    }
    return NULL;
}

/*
 * heap_push() --Insert an item into the heap.
 *
 * Parameters:
 * heap --the heap
 * item --the item to insert
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int heap_push(Heap *heap, const void *item)
{
    if (heap != NULL && heap->n_used < heap->array.n_items)
    {
        memcpy(array_item(&heap->array, heap->n_used),
               item, heap->array.item_size);
        ++heap->n_used;
        heap_sift_up(heap->array.base,
                     heap->n_used, heap->array.item_size, heap->cmp);
        return 1;                      /* success: item added to heap */
    }
    return 0;                          /* failure: no heap, or overflow */
}

/*
 * heap_pop() --Remove an item from the top of the heap.
 *
 * Parameters:
 * heap --the heap
 * item --returns a copy of the removed item
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int heap_pop(Heap *heap, void *item)
{
    if (heap_peek(heap, item) != NULL)
    {
        --heap->n_used;
        memcpy(heap->array.base,
               array_item(&heap->array, heap->n_used),
               heap->array.item_size);
        heap_sift_down(heap->array.base, 0, heap->n_used, heap->array.item_size,
                       heap->cmp);
        return 1;                      /* success */
    }
    return 0;                          /* failure: no heap, or underflow */
}

/*
 * heap_peek() --Peek at the top item in the heap.
 *
 * Parameters:
 * heap --the heap
 * item --NULL, or the address to copy the top item.
 *
 * Returns: (void *)
 * Success: a pointer to the top of heap; Failure: NULL.
 */
void *heap_peek(Heap *heap, void *item)
{
    if (heap != NULL && heap->n_used > 0)
    {
        if (item != NULL)
        {
            memcpy(item, heap->array.base, heap->array.item_size);
        }
        return heap->array.base;       /* success */
    }
    return NULL;                       /* failure: no heap, or empty */
}

/*
 * heap_delete() --Delete an item from the heap.
 *
 * Parameters:
 * heap --the heap
 * slot --the slot number in the heap array
 */
void heap_delete(Heap *heap, size_t slot)
{
    void *item = array_item(&heap->array, slot);
    const void *last_item = array_item(&heap->array, heap->n_used - 1);

    memcpy(item, last_item, heap->array.item_size);
    heap->n_used -= 1;

    heap_sift_down(heap->array.base, slot,
                   heap->n_used, heap->array.item_size, heap->cmp);
}
