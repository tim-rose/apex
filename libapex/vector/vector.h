/*
 * VECTOR.H --Declarations for handling simple dynamic arrays.
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <apex.h>
#include <apex/slink.h>                 /* VisitProc */

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
#define NEW_VECTOR(_type_, _n_el_, _new_el_) \
    new_vector(sizeof(_type_), (_n_el_), (_new_el_))
    /*
     * VectorInfo   --Housekeeping data used to manage vectors.
     */
    typedef struct VectorInfo_t
    {
        size_t n_el;                   /* No. allocated elements in the vector */
        size_t n_used;                 /* No. elements actually being used */
        size_t el_size;                /* size of each element */
    } VectorInfo, *VectorInfoPtr;

    void *new_vector(size_t elsize, size_t n_el, void *new_el) WARN_UNUSED;
    void free_vector(void *vp);
    VectorInfoPtr vector_info(void *vector, VectorInfoPtr viptr);
    int vector_len(void *vp);
    void *visit_vector(void *vector, VisitProc visit, void *user_data);
    int search_vector(void *vector, void *el, CompareProc el_cmp,
                      bool *statusp);
    void *vector_add(void *vector, size_t n_el, void *new_el) WARN_UNUSED;
    void *vector_insert(void *vector, size_t offset, size_t n_el,
                        void *new_el) WARN_UNUSED;
    void *vector_delete(void *vector, size_t offset, size_t n_el) WARN_UNUSED;
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* VECTOR_H */
