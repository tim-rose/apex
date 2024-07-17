#include <apex/hash.h>

/*
 * hash_keyn_jenkins() --Bob Jenkins' hash.
 *
 * See Also:
 * http://burtleburtle.net/bob/hash/doobs.html
 */
unsigned long hash_keyn_jenkins(char *data, size_t n)
{
    unsigned int hash = 0;

    for (; n > 0; --n, ++data)
    {
        hash += (unsigned int) *data;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}
