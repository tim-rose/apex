/*
 * key-pjw.c --An implementation of P.J. Weinberger's hashing algorithm
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 */
#include <limits.h>
#include <apex/hash.h>

#define BITS_PER_INT    (sizeof(int)*CHAR_BIT)
#define THREE_QUARTERS  ((int) (BITS_PER_INT*3)/4)
#define ONE_EIGHTH      ((int) (BITS_PER_INT)/8)
#define HIGH_BITS       (~((unsigned int) (~0) >> ONE_EIGHTH))

/*
 * hash_key_pjw() --P.J.Weinberger's generic hashing algorithm.
 *
 * Parameters:
 * data --the data to be hashed
 *
 * Returns: (unsigned int)
 * The hash value.
 */
unsigned long hash_key_pjw(char *data)
{
    unsigned int hash = 0, i;

    for (; *data; ++data)
    {
        hash = (hash << ONE_EIGHTH) + (unsigned int) *data;
        if ((i = hash & HIGH_BITS) != 0)
        {
            hash = (hash ^ (i >> THREE_QUARTERS)) & ~HIGH_BITS;
        }
    }
    return (hash);
}
