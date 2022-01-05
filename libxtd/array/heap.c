/*
 * HEAP.C --A simple heap array, with caller provided item storage.
 */
#include <memory.h>
#include <heap.h>

/*
 * heap_alloc() --Allocate some space for a heap structure.
 *
 * Returns: (HeapPtr)
 * Success: the allocated memory; Failure: NULL.
 */
/* LCOV_EXCL_START */
HeapPtr heap_alloc()
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
 * items --the storage for the heap (n_items*item_size)
 *
 * Returns:
 * Success: The heap; Failure: NULL.
 *
 * Remarks:
 * The heap storage is not allocated by this module, it must be provided
 * by the caller.
 */
HeapPtr heap_init(HeapPtr heap, CompareProc cmp, size_t n_items,
                  size_t item_size, void *items)
{
    if (heap != NULL && items != NULL)
    {
        heap->array.n_items = n_items;
        heap->array.item_size = item_size;
        heap->array.items = (char *) items;
        heap->n_used = 0;
        heap->cmp = cmp;
        return heap;
    }
    return NULL;
}

/*
 * heap_push() --Insert an item onto the heap.
 *
 * Parameters:
 * heap --the heap
 * item --address of the item to insert
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int heap_push(HeapPtr heap, const void *item)
{
    if (heap != NULL && heap->n_used < heap->array.n_items)
    {
        memcpy(heap->array.items +
               heap->n_used * heap->array.item_size,
               item, heap->array.item_size);
        ++heap->n_used;
        heap_sift_up(heap->array.items,
                     heap->array.n_items, heap->array.item_size, heap->cmp);
    }
    return 0;                          /* failure: no heap, or overflow */
}

/*
 * heap_pop() --Remove an item from the heap.
 *
 * Parameters:
 * heap --the heap
 * item --returns the removed item
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int heap_pop(HeapPtr heap, void *item)
{
    if (heap != NULL && heap->n_used > 0)
    {
        memcpy(item, heap->array.items, heap->array.item_size);
        --heap->n_used;
        memcpy(heap->array.items,
               heap->array.items +
               heap->n_used * heap->array.item_size, heap->array.item_size);
        heap_sift_down(heap->array.items,
                       heap->array.n_items, heap->array.item_size, heap->cmp);
        return 1;
    }
    return 0;                          /* failure: no heap, or underflow */
}

/*
 * heap_peek() --Peek at the top item in the heap.
 *
 * Parameters:
 * heap --the heap
 *
 * Returns: (void *)
 * Success: a pointer to the top of heap; Failure: NULL.
 */
void *heap_peek(HeapPtr heap)
{
    if (heap != NULL && heap->n_used > 0)
    {
        return heap->array.items;
    }
    return NULL;                       /* failure: no heap, or empty */
}
