/*
 * POOL.C --A simple pool allocator, with caller provided item storage.
 *
 * Contents:
 * pool_alloc()  --Allocate some space for a pool structure.
 * pool_init()   --Initialise a pool structure.
 * pool_new()    --Return a new item from the pool.
 * pool_delete() --Return an item to the pool.
 *
 * Remarks:
 * These routines manage a chunk of storage as an array of fixed size
 * items.  Requested items are initially allocated from the provided
 * memory, but freed (um, deleted()) items are managed as a simple
 * linked list which is used to satisfy storage requests.
 *
 * A pool allocator can be useful when a number of temporary objects
 * are needed; the aggregate can be discarded simply by freeing the
 * slab of memory controlled by the pool.
 */
#include <memory.h>
#include <pool.h>

typedef struct Link_t
{
    struct Link_t *next;
} Link, *LinkPtr;

/*
 * pool_alloc() --Allocate some space for a pool structure.
 *
 * Returns: (PoolPtr)
 * Success: the allocated memory; Failure: NULL.
 */
/* LCOV_EXCL_START */
PoolPtr pool_alloc()
{
    return malloc(sizeof(Pool));
}

/* LCOV_EXCL_STOP */

/*
 * pool_init() --Initialise a pool structure.
 *
 * Parameters:
 * pool --specifies and returns the initialised pool
 * n_items --the number of pool items
 * item_size --the size of each item
 * items --the storage for the pool (n_items*item_size)
 *
 * Returns: (Poolptr)
 * The pool.
 *
 * Remarks:
 * The pool storage is not allocated by this module, it is provided
 * by the caller.
 */
PoolPtr pool_init(PoolPtr pool, size_t n_items, size_t item_size,
                    void *items)
{
    if (pool != NULL && items != NULL
        && n_items > 0
        && item_size >= sizeof(LinkPtr))
    {
        memset(pool, 0, sizeof(*pool));
        pool->array.n_items = n_items;
        pool->array.item_size = item_size;
        pool->array.items = (char *) items;
        return pool;
    }
    return NULL;                        /* failure: assert? */
}

/*
 * pool_new() --Return a new item from the pool.
 *
 * Parameters:
 * pool --the pool
 *
 * Returns: (void *)
 * Success: the newly allocated memory; Failure: NULL.
 */
void *pool_new(PoolPtr pool)
{
    if (pool != NULL)
    {
        if (pool->free != NULL)
        {
            LinkPtr head = (LinkPtr) pool->free;
            pool->free = head->next;
            return (void *) head; /* success: return from free list */
        }
        if (pool->n_used < pool->array.n_items)
        {
            return pool->array.items
                + pool->array.item_size * pool->n_used++;
        }
    }
    return NULL;                          /* failure: the pool is empty! */
}

/*
 * pool_delete() --Return an item to the pool.
 *
 * Parameters:
 * pool --the pool
 * item --the item to delete
 */
void pool_delete(PoolPtr pool, void *item)
{
    if (pool != NULL)
    {
        LinkPtr link = (LinkPtr) item;

        link->next = (LinkPtr) pool->free;
        pool->free = link;
    }
}
