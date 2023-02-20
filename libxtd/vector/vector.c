/*
 * VECTOR.C --A C Implementation of dynamic arrays.
 *
 * Contents:
 * GET_VECTOR()    --Macro to get back to base of Vector struct.
 * new_vector()    --Allocate a new Vector structure.
 * free_vector()   --Free an existing Vector structure.
 * vector_info()   --Get the private information about a vector structure.
 * vector_len()    --Return the usable length of a Vector.
 * search_vector() --Find an array entry using "binary chop".
 * visit_vector()  --Visit every element in a vector.
 * vector_add()    --Add some elements to a vector's array.
 * vector_insert() --Add some elements to a vector at a specific offset.
 * vector_delete() --Delete items from a Vector's array.
 *
 * Remarks:
 * The vector module provides a simple implementation of dynamically
 * sized arrays.  Vectors are dynamically allocated at run-time, and
 * may be re-sized and extended as required.  The house-keeping data
 * is maintained in a block stored as a prefix to the array proper, so
 * that it is effectively hidden from the user.  Intrepid users may
 * wish to inspect this data; vector_info() provides a safe method
 * for doing so.
 *
 * Calls to vector_add() and vector_insert() will extend the allocated
 * size of the vector as required.  To minimise malloc() thrashing,
 * vector-arrays are extended in geometrically increasing chunks.
 *
 */
#include <stddef.h>
#include <string.h>

#include <xtd/log.h>
#include <xtd/binsearch.h>
#include <xtd/vector.h>

#define VECTOR_ALIGN	8              /* alignment in bytes */

/*
 * GET_VECTOR() --Macro to get back to base of Vector struct.
 */
#define GET_VECTOR(_v)	(VectorPtr) ((char*) _v - offsetof(Vector, vector))

/*
 * @type Vector The prefix block containing vector housekeeping data.
 */
typedef struct Vector_t
{
    VectorInfo info;
    char padding[VECTOR_ALIGN - (sizeof(VectorInfo) % VECTOR_ALIGN)];
    char vector[];                     /* the actual array */
} Vector, *VectorPtr;

/*
 * new_vector() --Allocate a new Vector structure.
 *
 * Parameters:
 * el_size   --the size of each element
 * n_el  -- an initial allocation of elements
 * new_el    --the elements to initialise with, or NULL
 *
 * Returns: (void *)
 * Success: pointer to the array part of the Vector; Failure: NULL.
 *
 * Vector arrays are malloc'd with a bit of houskeeping space at the front
 * to keep track of how much of the space is used. This stuff is effectively
 * hidden from the caller.
 */
void *new_vector(size_t el_size, size_t n_el, void *new_el)
{
    VectorPtr vector;
    size_t n_allocated = n_el;         /* No. allocated elements */

    if (n_allocated < 8)
    {
        n_allocated = 8;               /* require at least 8 elements */
    }
    if ((vector =
         (VectorPtr) malloc(sizeof(Vector) + n_allocated * el_size)) ==
        (VectorPtr) NULL)
    {
        return NULL;
    }
    vector->info.n_el = n_allocated;
    vector->info.n_used = n_el;
    vector->info.el_size = el_size;
    if (n_el && new_el)
    {
        memcpy(vector->vector, new_el, n_el * el_size);
    }
    return (vector->vector);
}

/*
 * free_vector() --Free an existing Vector structure.
 *
 * Parameter:
 * vector   --pointer to the array part of a Vector
 */
void free_vector(void *vector)
{
    if (vector)
    {
        free((void *) GET_VECTOR(vector));
    }
}

/*
 * vector_info() --Get the private information about a vector structure.
 *
 * Parameters:
 * vector   --pointer to the array part of a Vector
 * viptr  --address of a buffer to copy the information to
 *
 * Returns: (VectorInfoPtr) viptr.
 */
VectorInfoPtr vector_info(void *vector, VectorInfoPtr viptr)
{
    VectorPtr v = GET_VECTOR(vector);

    return memcpy(viptr, &v->info, sizeof(v->info));
}

/*
 * vector_len() --Return the usable length of a Vector.
 *
 * Parameter:
 * vector   --pointer to the array part of a Vector
 *
 * Returns: (int)
 * Success: the length of the table; Failure: -1.
 *
 * Remarks:
 * vector_len() returns the "user visible" length, not the malloc'd length.
 *
 * See Also: vector_info().
 */
int vector_len(void *vector)
{
    VectorPtr v = GET_VECTOR(vector);

    if (!vector)
    {
        return (-1);                   /* failure: null pointer */
    }
    return v->info.n_used;
}

/*
 * search_vector() --Find an array entry using "binary chop".
 *
 * Parameters:
 * vector   --     pointer to the table part of a VECTOR
 * el   --     the element to find
 * elcmp    --  an element comparison function
 * statusp  --returns the status of the search
 *
 * Returns: (size_t)
 * The offset in the table, ala binsearch().
 *
 * Remarks:
 * Obviously, this will only work if the table is maintained in
 * sorted order.
 */
int search_vector(void *vector, void *el, CompareProc elcmp, bool *statusp)
{
    VectorPtr t = GET_VECTOR(vector);

    return binsearch(el, t->vector, t->info.n_used, t->info.el_size,
                     elcmp, statusp);
}

/*
 * visit_vector() --Visit every element in a vector.
 *
 * Parameters:
 * vector   --     pointer to the table part of a VECTOR
 * el   --     the element to find
 * elcmp    --  an element comparison function
 * statusp  --returns the status of the search
 *
 * Returns: (void *)
 * The item "selected" by visit(), or NULL.
 *
 * Remarks:
 * visit_vector() will call the provided visit() function with every
 * item on the list until visit() returns non-null.
 */
void *visit_vector(void *vector, VisitProc visit, void *usrdata)
{
    VectorPtr v = GET_VECTOR(vector);
    size_t n = v->info.n_used;

    for (size_t i = 0; i < n; ++i)
    {
        void *item = v->vector + i * v->info.el_size;

        if (visit(item, usrdata) != 0)
        {
            return item;
        }
    }
    return NULL;
}

/*
 * vector_add() --Add some elements to a vector's array.
 *
 * Parameters:
 * vector   -- pointer to the array part of a vector
 * n_el  --    the number of elements to add
 * new_el    --  the new elements to add, or NULL
 *
 * Returns: (void *)
 * Success: The (possibly realloc'd) vector; Failure: NULL.
 */
void *vector_add(void *vector, size_t n_el, void *new_el)
{
    return (vector_insert(vector, (size_t) vector_len(vector), n_el, new_el));
}

/*
 * vector_insert() --Add some elements to a vector at a specific offset.
 *
 * Parameters:
 * vector   -- pointer to the array part of a vector
 * offset   -- minsertion offset in the vector
 * n_el  --    the number of elements to add
 * new_el    --  the new elements to add, or NULL
 *
 * Returns: (void *)
 * Success: The (possibly realloc'd) vector; Failure: NULL.
 *
 * Remarks:
 * vector_insert() inserts items into the vector, expanding it as
 * necessary.  Expansion is done geometrically:  The size of the
 * malloc'd buffer is increased by at least 50% each time.  This is
 * important, because linear expansion would lead to malloc-thrashing
 * and O(n^2) performance for some applications of Vector.
 */
void *vector_insert(void *vector, size_t offset, size_t n_el, void *new_el)
{
    VectorPtr v = GET_VECTOR(vector), new_vector;

    if (vector == NULL)
    {
        return NULL;                   /* failure: bad arguments */
    }

    if (v->info.n_used + n_el > v->info.n_el)
    {                                  /* not enough slots for n_el items */
        size_t n = (v->info.n_el / 2) * 3;  /* geometric expansion... */
        if (n < v->info.n_used + n_el)
        {                              /* ...but caller wants more */
            n = v->info.n_used + n_el;
        }
        n += 16 - n % 16;              /* increment in modulo 16 chunks */
        debug("realloc(): was %zu, now %zu", v->info.n_el, n);
        if ((new_vector = realloc(v, sizeof(Vector) + n * v->info.el_size))
            == NULL)
        {
            free(v);
            return NULL;               /* failure: realloc() problem */
        }
        v = new_vector;
        v->info.n_el = n;
        vector = v->vector;
    }

    if (offset > v->info.n_used)
    {                                  /* zero fill un_used elements */
        memset(v->vector + v->info.n_used * v->info.el_size, 0,
               ((offset - v->info.n_used) * v->info.el_size));
        v->info.n_used = offset;
    }
    if (offset != v->info.n_used)
    {                                  /* make room as necessary */
        memmove(v->vector + (offset + n_el) * v->info.el_size,  /* destination */
                v->vector + offset * v->info.el_size,   /* source */
                (v->info.n_used - offset) * v->info.el_size);   /* No. bytes */
    }
    if (new_el)
    {
        memmove(v->vector + offset * v->info.el_size, new_el,
                n_el * v->info.el_size);
    }
    v->info.n_used += n_el;
    return vector;                     /* success */
}

/*
 * vector_delete() --Delete items from a Vector's array.
 *
 * Parameters:
 * vector   -- pointer to the array part of a vector
 * offset   --offset in the table of first element to remove
 * n_el  -- the number of elements to remove
 *
 * Returns: (void *)
 * Success: The (possibly realloc'd) vector; Failure: NULL.
 *
 * Remarks:
 * vector_delete() removes the specified items from the vector, and
 * adjusts the memory allocation to suit.  Items deleted from the
 * middle of the vector cause the upper items to be copied down.
 *
 * As with vector_insert(), the memory is re-allocated geometrically; the
 * allocation is shrunk when the vector is 50% empty.
 *
 * See Also: vector_insert().
 */
void *vector_delete(void *vector, size_t offset, size_t n_el)
{
    VectorPtr v = GET_VECTOR(vector);

    if (!vector)
    {
        return NULL;                   /* failure: nothing to delete from! */
    }
    if (offset + n_el > v->info.n_used)
    {                                  /* limit arguments to current bounds */
        n_el = v->info.n_el - offset;
    }
    memmove(v->vector + offset * v->info.el_size,
            v->vector + (offset + n_el) * v->info.el_size,
            (v->info.n_used - offset - n_el) * v->info.el_size);
    v->info.n_used -= n_el;
    if (v->info.n_used < v->info.n_el / 2)
    {                                  /* shrink allocation */
        VectorPtr new_vector
            = realloc(v, sizeof(Vector) + v->info.n_used * v->info.el_size);
        if (new_vector == NULL)
        {
            free(v);
            return NULL;               /* failure: realloc */
        }
        v = new_vector;
        v->info.n_el = v->info.n_used;
    }
    return v->vector;
}
