/*
 * HEAP.C --A simple heap container, with caller provided item storage.
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
HeapPtr heap_init(HeapPtr heap, CompareProc cmp, size_t n_items, size_t item_size, void *items)
{
    if (heap != NULL && items != NULL)
    {
        heap->container.n_items = n_items;
        heap->container.item_size = item_size;
        heap->container.items = (char *) items;
        heap->n_used = 0;
        heap->cmp = cmp;
        return heap;
    }
    return NULL;
}

/*
 * heap_insert() --Insert an item onto the heap.
 *
 * Parameters:
 * heap --the heap
 * item --the item to insert
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int heap_insert(HeapPtr heap, const void *item)
{
    if (heap != NULL)
    {
    }
    return 0;                           /* failure: no heap, or overflow */
}

/*
 * heap_remove() --Remove an item from the heap.
 *
 * Parameters:
 * heap --the heap
 * item --returns the removeped item
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int heap_remove(HeapPtr heap, void *item)
{
    if (heap != NULL)
    {
    }
    return 0;                           /* failure: no heap, or underflow */
}

/*
 * heap_peek() --Peek at the top item in the heap.
 *
 * Parameters:
 * heap --the heap
 *
 * Returns: (void *)
 * Success: a pointer to the top of heap; Failure: NULL.
 *
 * Remarks:
 * Consider allowing peek at lower heap items too?
 */
void *heap_peek(HeapPtr heap)
{
    if (heap != NULL)
    {
    }
    return NULL;                           /* failure: no heap, or empty */
}
