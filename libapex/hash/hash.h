/*
 * HASH.H --Definitions for a hash functions and a simple hash-table.
 *
 */
#ifndef HASH_H
#define HASH_H

#include <stdbool.h>

#include <apex/clink.h>
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */

    /*
     * HashProc --A function to determine the hash-key from some data.
     *
     * Remarks:
     * The value returned by this function will be remaindered with the
     * hash table size.
     */
    typedef unsigned long (*HashProc)(char *data);

    /*
     * Hash     --Fixed size hash table structure.
     *
     * Fields:
     * hash -- the hashing function used to spread items into the hash table slots.
     * nslot --the number of slots in the hash table
     * slot --  an array of slots for holding user data
     */
    typedef struct Hash_t              /* Hash table housekeeping data */
    {                                  /* (this should prob. be private */
        HashProc hash;
        size_t nslot;
        LinkPtr slot[];                /* collision handled by linked list */
    } Hash, *HashPtr;

    HashPtr hash_new(HashProc hash, size_t nslot);
    void hash_free(HashPtr h);
    bool hash_insert(HashPtr h, void *data);
    void *hash_remove(HashPtr h, CompareProc cmp, void *key);
    void *hash_find(HashPtr h, CompareProc cmp, void *key);
    void *hash_visit(HashPtr h, VisitProc visit, void *user_data);

    unsigned long hash_key_pjw(char *data);
    unsigned long hash_keyn_pjw(char *data, size_t n);
    unsigned long hash_key_elf(char *data);
    unsigned long hash_keyn_elf(char *data, size_t n);
    unsigned long hash_key_jenkins(char *data);
    unsigned long hash_keyn_jenkins(char *data, size_t n);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* HASH_H */
