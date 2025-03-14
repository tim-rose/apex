/*
 * POOL.H --A simple pool allocator, with caller provided item storage.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 */
#ifndef APEX_POOL_H
#define APEX_POOL_H

#include <apex/array.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct Pool
    {
        ArrayContainer array;
        size_t n_used;                    /* maximum number of used items */
        void *free;                    /* current list of freed items. */
    } Pool;

    Pool *pool_alloc(void);
    Pool *pool_init(Pool *pool, size_t n_items, size_t item_size, void *items);

    void *pool_new(Pool *pool);
    void pool_delete(Pool *pool, void *item);


    /*
     * Convenience functions for malloc and item-size aware initialisation.
     */
#define new_pool(items) init_pool(pool_alloc(), items)
#define init_pool(pool, items) pool_init(pool, NEL(items), sizeof(items[0]), items)
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_POOL_H */
