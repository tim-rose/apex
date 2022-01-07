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
        size_t n_used;
        void *free;
    } Pool, *PoolPtr;

    PoolPtr pool_alloc(void);
    PoolPtr pool_init(PoolPtr pool, size_t n_items,
                        size_t item_size, void *items);

    void *pool_new(PoolPtr pool);
    void pool_delete(PoolPtr pool, void *item);

#define new_pool(items) init_pool(pool_alloc(), items)
#define init_pool(pool, items) pool_init(pool, NEL(items), sizeof(items[0]), items)
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* POOL_H */
