/*
 * POOL.C --A simple pool allocator, with caller provided item storage.
 *
 * Contents:
 * Link{}        --free list record
 * pool_alloc()  --Allocate some space for a pool structure.
 * pool_init()   --Initialise a pool structure.
 * pool_new()    --Get a new item from the pool.
 * pool_delete() --Return an item to the pool.
 *
 * Remarks:
 * These routines manage a chunk of storage as an array of fixed size
 * items.  Requested items are initially allocated from the provided
 * memory, but freed (um, deleted()) items are managed as a simple
 * linked list which is used to satisfy storage requests if possible.
 *
 * A pool allocator can be useful when a number of temporary objects
 * are needed; the aggregate can be discarded simply by freeing the
 * slab of memory controlled by the pool.
 *
 * Pool allocator also has well known (and good!) performance, because
 * it avoids fragmentation and merging etc. that a general purpose
 * allocator must do.
 */
#include <memory.h>
#include <apex/pool.h>

/*
 * Link{} --free list record
 */
typedef struct Link
{
    struct Link_t *next;
} Link;

/*
 * pool_alloc() --Allocate some space for a pool structure.
 *
 * Returns: (Pool *)
 * Success: the allocated memory; Failure: NULL.
 */
/* LCOV_EXCL_START */
Pool *pool_alloc(void)
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
 * base --the storage for the pool (size == n_items*item_size)
 *
 * Returns: (Poolptr)
 * The pool.
 *
 * Remarks:
 * The pool storage is not allocated by this module, it is provided
 * by the caller.
 */
Pool *pool_init(Pool *pool, size_t n_items, size_t item_size, void *base)
{
    if (pool != NULL && base != NULL
        && n_items > 0 && item_size >= sizeof(Link *))
    {
        memset(pool, 0, sizeof(*pool));
        array_init(&pool->array, n_items, item_size, base);
        return pool;
    }
    return NULL;                       /* failure: assert? */
}

/*
 * pool_new() --Get a new item from the pool.
 *
 * Parameters:
 * pool --the pool to allocate from
 *
 * Returns: (void *)
 * Success: the newly allocated memory; Failure: NULL.
 */
void *pool_new(Pool *pool)
{
    if (pool != NULL)
    {
        if (pool->free != NULL)
        {                              /* try free-list first. */
            Link *head = (Link *) pool->free;

            pool->free = head->next;
            return (void *) head;      /* success: return from free list */
        }
        if (pool->n_used < pool->array.n_items)
        {
            return pool->array.base + pool->array.item_size * pool->n_used++;
        }
    }
    return NULL;                       /* failure: the pool is empty! */
}

/*
 * pool_delete() --Return an item to the pool.
 *
 * Parameters:
 * pool --the pool
 * item --the item to delete
 *
 * Remarks:
 * REVIISIT: This routine could validate the address being returned.
 */
void pool_delete(Pool *pool, void *item)
{
    if (pool != NULL)
    {
        Link *link = (Link *) item;

        link->next = (Link *) pool->free;
        pool->free = link;
    }
}
