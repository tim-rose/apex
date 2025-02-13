/*
 * HEAP-SIFT.C --Fundamental operations for implicit (array) heaps.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * heap_sift_up()   --Sift a value from the bottom of the heap to the top.
 * heap_sift_down() --Sift a value from the top to its "correct" position.
 * heap_ok()        --Check the heap condition.
 *
 * Remarks:
 * The heap data structure is a semi-ordered tree that maintains the
 * property that a parent is greater/equal than ALL its descendents,
 * with no ordering specified between siblings.  It is commonly
 * modelled as an "implicit" binary tree, implemented as a simple
 * array.  An array implementation is possible because binary-tree
 * heaps are inherently "balanced" (maybe "full-ish" is a better term
 * here), so there are no "holes" in the tree.
 */

#include <string.h>
#include <apex/heap.h>
#include <apex/estring.h>               /* for memswap() */

/*
 * heap_sift_up() --Sift a value from the bottom of the heap to the top.
 *
 * Parameters:
 * heap --specifies the heap array
 * n_items  --No. items in the heap
 * item_size    --size of each item
 * cmp  --comparison function
 *
 * Remarks:
 * A heap "sift-up" is usually used after an insertion: the item is
 * appended to the end of the list, creating a heap that is valid
 * *except* for this last leaf item. sift-up re-creates the heap
 * condition by "floating" the value up the heap towards root.
 */
void heap_sift_up(void *heap, size_t n_items, size_t item_size, CompareProc cmp)
{
    for (size_t i = n_items - 1; i > 0; i = (i - 1) / 2)
    {
        char *node = (char *) heap + i * item_size;
        char *parent = (char *) heap + (i - 1) / 2 * item_size;

        if (cmp(node, parent) < 0)
        {
            memswap(parent, node, (size_t) item_size);
        }
    }
}

/*
 * heap_sift_down() --Sift a value from the top to its "correct" position.
 *
 * Parameters:
 * heap --specifies the heap array
 * n_items  --No. items in the heap
 * item_size    --size of each item
 * cmp  --comparison function
 *
 * Remarks:
 * A heap "sift-down" is usually used after a deletion: the first item
 * is removed, and the last item is swapped into its place, reducing
 * the heap by 1. The sift-down re-creates the heap condition by
 * pushing the root down to the child slots.
 */
void heap_sift_down(void *heap, size_t slot, size_t n_items, size_t item_size, CompareProc cmp)
{
    size_t level = 1;
    char *node, *child;
    const char *end;

    for (size_t id = slot; id > 0; id /= 2)
    {                                   /* find tree level of the item */
        level *= 2;
    }

    node = (char *) heap + slot * item_size;
    end = (char *) heap + n_items * item_size;
    for (; node < end; node = child, level *= 2)
    {
        child = (char *) node + level * item_size;  /* left child */
        if (child >= end)
        {
            break;
        }

        char *alt_child = child + item_size;    /* right child */

        if (alt_child < end && cmp(child, alt_child) > 0)
        {                              /* choose smallest child */
            child = alt_child;
        }
        if (cmp(child, node) > 0)
        {
            break;
        }
        memswap(node, child, (size_t) item_size);
    }
}

/*
 * heap_ok() --Check the heap condition.
 *
 * Parameters:
 * heap --specifies the heap array
 * n_items  --No. items in the heap
 * item_size    --size of each item
 * cmp  --comparison function
 *
 * Remarks:
 * This function is used for unit testing.
 */
int heap_ok(void *heap, size_t n_items, size_t item_size, CompareProc cmp)
{
    for (size_t i = n_items - 1; i > 0; --i)
    {
        char *node, *parent;

        node = (char *) heap + i * item_size;
        parent = (char *) heap + (i - 1) / 2 * item_size;
        if (cmp(node, parent) < 0)
        {
            return 0;
        }
    }
    return 1;
}
