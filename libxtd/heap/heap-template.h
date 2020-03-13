/*
 * HEAP-TEMPLATE.H --Macros for defining type-specific heap functions.
 *
 * Contents:
 * HEAP_SIFT_UP_PROC()  --define a function that does a heap "sift-up".
 * HEAP_SIFT_DOWN_PROC() --define a function that does a heap "sift-down".
 * HEAP_OK_PROC()       --define a function that checks the heap condition.
 *
 * Remarks:
 * This file defines some CPP macros that define functions used
 * for manipulating heap state using the "classic" array implementation
 * of heaps.  This is about as close as C can get to C++'s
 * template functions.
 *
 */
#ifndef HEAP_TEMPLATE_H
#define HEAP_TEMPLATE_H

/*
 * HEAP_SIFT_UP_PROC() --define a function that does a heap "sift-up".
 *
 * Parameters:
 * name --the name of the function to define
 * type --the type of elements being managed in the heap
 * compare --the comparison function (or macro!)
 *
 * Remarks:
 * A heap "sift-up" is usually used after an insertion: the item is
 * appended to the end of the list and then the sift-up re-creates the
 * heap condition.
 */
#define HEAP_SIFT_UP_PROC(_name_, _type_, _compare_) \
static void _name_(_type_ value[], unsigned int nel) \
{ \
    _type_  tmp, *parent, *node; \
 \
    for (unsigned int i = nel-1; i > 0; i = (i-1)/2) \
    { \
        node = value + i; \
        parent = value + (i-1)/2; \
        if (_compare_(node, parent) < 0) { \
            tmp = *parent; *parent = *node; *node = tmp; \
        } \
    } \
}

/*
 * HEAP_SIFT_DOWN_PROC() --define a function that does a heap "sift-down".
 *
 * Parameters:
 * name --the name of the function to define
 * type --the type of elements being managed in the heap
 * compare --the comparison function (or macro!)
 *
 * Remarks:
 * A heap "sift-down" is usually used after a deletion: the first item
 * is removed, and the last item is swapped into its place.  The
 * sift-down re-creates the heap condition.
 */
#define HEAP_SIFT_DOWN_PROC(_name_, _type_, _compare_) \
static void _name_(_type_ value[], unsigned int nel) \
{ \
    int level; \
    _type_  tmp, *node, *child, *alt_child, *end; \
 \
    end = value + nel; \
    for (level = 1, node = value; node < end; node = child, level *= 2)  \
    { \
        child = node+level;               /* left child */    \
        if (child >= end) break; \
        alt_child = child+1;            /* right child */ \
        if (alt_child < end && _compare_(child, alt_child) > 0) \
        { \
            child = alt_child; \
        } \
        if (_compare_(child, node) > 0) break; \
        tmp = *node; *node = *child; *child = *node; \
    } \
}

/*
 * HEAP_OK_PROC() --define a function that checks the heap condition.
 *
 * Parameters:
 * name --the name of the function to define
 * type --the type of elements being managed in the heap
 * compare --the comparison function (or macro!)
 *
 * Remarks:
 * This function is used for unit testing.
 */
#define HEAP_OK_PROC(_name_, _type_, _compare_) \
static int _name_(_type_ value[], unsigned int nel) \
{ \
    _type_ *node, *parent; \
     \
    for (unsigned int i=nel-1; i>0; --i) \
    { \
        node = value + i; \
        parent = value + (i-1)/2; \
        if (_compare_(node, parent) < 0) \
        { \
            return 0; \
        } \
    } \
    return 1; \
}
#endif /* HEAP_TEMPLATE_H */
