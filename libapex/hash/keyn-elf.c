#include <apex/hash.h>

/*
 * hash_key_elf() --UNIX ELF's hashing algorithm.
 *
 * Parameters:
 * data --the data to be hashed
 *
 * Returns: (unsigned long)
 * The hash value.
 *
 * Remarks:
 * Does this work for 64-bit?
 */
unsigned long hash_keyn_elf(char *data, size_t n)
{
    unsigned int hash = 0, i;

    for (; n > 0; --n, ++data)
    {
        hash = (hash << 4) + (unsigned int) *data;
        if ((i = hash & 0xF0000000) != 0)
            hash ^= i >> 24;
        hash &= ~i;
    }
    return (hash);
}
