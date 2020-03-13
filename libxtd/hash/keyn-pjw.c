/*
 * keyn-pjw.c --An implementation of P.J. Weinberger's hashing algorithm
 */
#include <limits.h>
#include <hash.h>

#define BITS_PER_INT    (sizeof(int)*CHAR_BIT)
#define THREE_QUARTERS  ((int) (BITS_PER_INT*3)/4)
#define ONE_EIGHTH      ((int) (BITS_PER_INT)/8)
#define LOW_BITS       ((unsigned int) (~0) >> ONE_EIGHTH)
#define HIGH_BITS       (~LOW_BITS)

/*
 * hash_keyn_pjw() --P.J.Weinberger's generic hashing algorithm.
 *
 * Parameters:
 * data --the data to be hashed
 * n    --the number of bytes of `data` to hash
 *
 * Returns: (unsigned int)
 * The hash value.
 */
unsigned long hash_keyn_pjw(char *data, size_t n_bytes)
{
    unsigned long hash = 0, high_bits;

    for (; n_bytes > 0; ++data, --n_bytes)
    {
        hash = (hash << ONE_EIGHTH) + (unsigned int) *data;
        high_bits = hash & HIGH_BITS;
        if (high_bits != 0)
        {
            hash = (hash ^ (high_bits >> THREE_QUARTERS)) & LOW_BITS;
        }
    }
    return (hash);
}
