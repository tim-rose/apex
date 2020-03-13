/*
 * HEAP.C --Routines for managing implicit (array) heap data structures.
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
 * modelled as an "implicit" binary tree, and implemented as a simple
 * array.  An array implementation is possible because binary-tree
 * heaps are inherently "balanced" (maybe "full" is a better term
 * here), so there are no "holes" in the tree.
 *
 */

#include <string.h>
#include <heap.h>
#include <estring.h>

/*
 * heap_sift_up() --Sift a value from the bottom of the heap to the top.
 *
 * Parameters:
 *
 * Remarks:
 * A heap "sift-up" is usually used after an insertion: the item is
 * appended to the end of the list and then the sift-up re-creates the
 * heap condition.
 */
void heap_sift_up(void *base, size_t nel, size_t elsize, CompareProc cmp)
{
    char *parent, *node;

    for (size_t i = nel - 1; i > 0; i = (i - 1) / 2)
    {
        node = (char *) base + i * elsize;
        parent = (char *) base + (i - 1) / 2 * elsize;
        if (cmp(node, parent) < 0)
        {
            memswap(parent, node, elsize);
        }
    }
}

/*
 * heap_sift_down() --Sift a value from the top to its "correct" position.
 *
 * Parameters:
 *
 * Remarks:
 * A heap "sift-down" is usually used after a deletion: the first item
 * is removed, and the last item is swapped into its place.  The
 * sift-down re-creates the heap condition.
 */
void heap_sift_down(void *base, size_t nel, size_t elsize, CompareProc cmp)
{
    size_t level;
    char *node, *child, *alt_child, *end;

    end = (char *) base + nel * elsize;
    for (level = 1, node = base; node < end; node = child, level *= 2)
    {
        child = (char *) node + level * elsize; /* left child */
        if (child >= end)
        {
            break;
        }
        alt_child = child + elsize;    /* right child */
        if (alt_child < end && cmp(child, alt_child) > 0)
        {                              /* choose smallest child */
            child = alt_child;
        }
        if (cmp(child, node) > 0)
        {
            break;
        }
        memswap(node, child, elsize);
    }
}

/*
 * heap_ok() --Check the heap condition.
 *
 * Parameters:
 * name --the name of the function to define
 * type --the type of elements being managed in the heap
 * compare --the comparison function (or macro!)
 *
 * Remarks:
 * This function is used for unit testing.
 */
int heap_ok(void *base, size_t nel, size_t elsize, CompareProc cmp)
{
    char *node, *parent;

    for (size_t i = nel - 1; i > 0; --i)
    {
        node = (char *) base + i * elsize;
        parent = (char *) base + (i - 1) / 2 * elsize;
        if (cmp(node, parent) < 0)
        {
            return 0;
        }
    }
    return 1;
}
