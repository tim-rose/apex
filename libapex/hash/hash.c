/*
 * HASH.C --A simple hash table implementation, and some sample hashing functions.
 *
 * Contents:
 * hash_new()    --Create a new hash table.
 * hash_free()   --Free a hash table, releasing all resources.
 * hash_insert() --Insert an item into a hash table.
 * hash_remove() --Remove an item from a hash table.
 * hash_find()   --Find a particular item in a hash table.
 * hash_visit()  --Visit all the items in a hash table.
 *
 * Remarks:
 * This hash table implementation constructs a fixed-size array of
 * hash slots when the hash is initialised.  It handles collisions by
 * storing user data in a linked list, one list per slot.
 *
 * References:
 * Andrew Binstock: Hashing Rehashed.
 * Dr. Dobb's Journal, April 1996, pp 24--33.
 *
 */
#include <stdlib.h>
#include <apex/hash.h>

/*
 * hash_new() --Create a new hash table.
 *
 * Parameters:
 * hash_proc --the hash function, used to allocate items to slots
 * nslot    -- the number of slots in the hashtable
 *
 * Returns: (Hash *)
 * Success: the hash table; Failure: NULL.
 */
Hash *hash_new(HashProc hash_proc, size_t nslot)
{
    Hash *hash = NULL;

    if (nslot > 0)
    {
        hash = (Hash *) malloc(sizeof(Hash) + nslot * sizeof(Link *));
        if (hash != NULL)
        {
            hash->hash = hash_proc;
            hash->nslot = nslot;
            for (size_t i = 0; i < nslot; ++i)
            {                          /* mark all slots as empty */
                hash->slot[i] = NULL;
            }
        }
    }
    return hash;
}

/*
 * hash_free() --Free a hash table, releasing all resources.
 *
 * Parameters:
 * hash    --the hash table to free
 *
 * Remarks:
 * It is the caller's responsibility to free any resources associated
 * with the data stored in the hash table itself.  This can be done by
 * traversing the hash's contents with a visitor function that cleans
 * up.  Something like:
 *
 *     static void *free_my_data(void *data, void *user_data)
 *     {
 *         free(data);
 *     }
 *     ...
 *     hash_visit(h, free_my_data, arg);
 *     hash_free(h);
 */
void hash_free(Hash *hash)
{
    size_t n_slot = hash->nslot;

    for (size_t i = 0; i < n_slot; ++i)
    {
        if (hash->slot[i] != NULL)
        {                              /* free up each list */
            link_free_links(hash->slot[i]->next, hash->slot[i]);
        }
    }
    free(hash);                        /* free the whole table */
}

/*
 * hash_insert() --Insert an item into a hash table.
 *
 * Parameters:
 * hash    --the hash table
 * data --the data to insert
 *
 * Returns: (int)
 * Success: true; Failure: false.
 */
bool hash_insert(Hash *hash, void *data)
{
    size_t i = hash->hash(data) % hash->nslot;
    Link *tail = hash->slot[i];
    Link *l;

    if ((l = link_new(tail, data)) == NULL)
    {
        return false;                  /* failure: malloc */
    }
    hash->slot[i] = clink_add(tail == NULL ? l : tail, l);
    return true;                       /* success */
}

/*
 * hash_remove() --Remove an item from a hash table.
 *
 * Parameters:
 * hash    --the hash table
 * cmp  --the comparison function to find the item
 * key  --a key describing the item to be removed
 *
 * Returns: (void *)
 * Success: the removed item; Failure: NULL.
 */
void *hash_remove(Hash *hash, CompareProc cmp, void *key)
{
    size_t key_slot = hash->hash(key) % hash->nslot;
    Link *tail = hash->slot[key_slot];

    if (tail != NULL)
    {
        Link *rlink;

        hash->slot[key_slot] = clink_remove(tail, cmp, key, &rlink);
        if (rlink != NULL)
        {
            void *value = rlink->data;

            link_free(rlink);
            return value;              /* success */
        }
    }
    return NULL;                       /* failure: empty slot or not found */
}

/*
 * hash_find() --Find a particular item in a hash table.
 *
 * Parameters:
 * h    --specifies the hash table
 * cmp  --the comparison function used to match the item
 * key  --the key value similar to the item sought
 *
 * Returns: (void *)
 * Success: the item; Failure: NULL.
 */
void *hash_find(Hash *hash, CompareProc cmp, void *key)
{
    size_t key_slot = hash->hash(key) % hash->nslot;
    Link *tail = hash->slot[key_slot];

    if (tail != NULL)
    {
        return clink_find(tail, cmp, key);
    }
    return NULL;                       /* failure: empty slot */
}

/*
 * hash_visit() --Visit all the items in a hash table.
 *
 * Parameters:
 * hash    --specifies the hash table
 * visit    --the visit function to call on every item
 * user_data  --miscellaneous data to call visit with.
 *
 * Returns: (void *)
 * The item "selected" by visit(), or NULL.
 *
 * Remarks:
 * hash_visit will call visit() with every item in the hash table
 * until visit() returns non-null; this value is passed
 * back to the caller.
 *
 * See Also: hash_find
 */
void *hash_visit(Hash *hash, VisitProc visit, void *user_data)
{
    size_t nslot = hash->nslot;

    for (size_t i = 0; i < nslot; ++i)
    {                                  /* for each slot... */
        void *value;
        Link *tail = hash->slot[i];

        if (tail != NULL &&
            (value = clink_visit(tail, visit, user_data)) != NULL)
        {                              /* LinkVisit() wants us to stop */
            return value;
        }
    }
    return NULL;
}
