/*
 * ARRAY.H --Array metadata management.
 *
 * Contents:
 * array_init() --Initialise an array container.
 * array_item() --Return the address of an array item.
 *
 * Remarks:
 * This is used as a base structure/class for other array-based data
 * structures (stack, heap, queue etc.).
 */
#ifndef ARRAY_H
#define ARRAY_H

#include <xtd.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct ArrayContainer_t
    {
        size_t n_items;
        size_t item_size;
        char *base;                    /* size: n_items*item_size */
    } ArrayContainer, *ArrayContainerPtr;

    /*
     * array_init() --Initialise an array container.
     */
    inline void array_init(ArrayContainerPtr array,
                           size_t n_items, size_t item_size, void *base)
    {
        array->n_items = n_items;
        array->item_size = item_size;
        array->base = (char *) base;
    }


    /*
     * array_item() --Return the address of an array item.
     */
    inline char *array_item(ArrayContainerPtr array, ssize_t offset)
    {
        return array->base + offset * (long) array->item_size;
    }
#ifdef __cplusplus
}
#endif /* C++ */
#endif /* ARRAY_H */
