/*
 * POOL.H --A simple pool allocator, with caller provided item storage.
 */
#ifndef POOL_H
#define POOL_H

#include <array.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct Pool_t
    {
        ArrayContainer array;
        int n_used;                 /* maximum number of used items */
        void *free;                    /* current list of freed items. */
    } Pool, *PoolPtr;

    PoolPtr pool_alloc(void);
    PoolPtr pool_init(PoolPtr pool, int n_items,
                      int item_size, void *items);

    void *pool_new(PoolPtr pool);
    void pool_delete(PoolPtr pool, void *item);


    /*
     * Convenience functions for malloc and item-size aware initialisation.
     */
#define new_pool(items) init_pool(pool_alloc(), items)
#define init_pool(pool, items) pool_init(pool, NEL(items), sizeof(items[0]), items)
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* POOL_H */
