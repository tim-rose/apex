#include <xtd/hash.h>
/*
 * hash_key_jenkins() --Bob Jenkins' hash.
 *
 * See Also:
 * http://burtleburtle.net/bob/hash/doobs.html
 */
unsigned long hash_key_jenkins(char *data)
{
    unsigned long hash;

    for (hash = 0; *data; ++data)
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
