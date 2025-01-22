/*
 * ARRAY.H --Array metadata management.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * array_init() --Initialise an array container.
 * array_item() --Return the address of an array item.
 *
 * Remarks:
 * This is used as a base structure/class for other slab/array data
 * structures (stack, heap, queue etc.).
 */
#ifndef APEX_ARRAY_H
#define APEX_ARRAY_H

#include <apex.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct ArrayContainer
    {
        size_t n_items;
        size_t item_size;
        char *base;                    /* size == n_items*item_size */
    } ArrayContainer;

    /*
     * array_init() --Initialise an array container.
     *
     * Parameters:
     * array --the array container to be initialised
     * n_items --the number of items in the container data
     * item_size --the size of each contained item
     * base --the base address of the array storage
     */
    static inline void array_init(ArrayContainer *array,
                                  size_t n_items, size_t item_size, void *base);
    static inline void array_init(ArrayContainer *array,
                                  size_t n_items, size_t item_size, void *base)
    {
        array->n_items = n_items;
        array->item_size = item_size;
        array->base = (char *) base;
    }

    /*
     * array_item() --Return the address of an array item.
     *
     * Parameters:
     * array --the array container
     * offset --the index/slot of the item
     *
     * Returns: (void *)
     * The address of item[offset].
     */
    static inline void *array_item(const ArrayContainer *array, size_t offset);
    static inline void *array_item(const ArrayContainer *array, size_t offset)
    {
        return array->base + offset * array->item_size;
    }
#ifdef __cplusplus
}
#endif /* C++ */
#endif /* APEX_ARRAY_H */
