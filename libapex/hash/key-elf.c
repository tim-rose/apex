/*
 * UNIX ELF hashing algorithm.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 */
#include <apex/hash.h>

/*
 * hash_key_elf() --UNIX ELF hashing algorithm.
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
unsigned long hash_key_elf(char *data)
{
    unsigned long hash, i;

    for (hash = 0; *data; ++data)
    {
        hash = (hash << 4) + (unsigned long) *data;
        if ((i = hash & 0xF0000000) != 0)
            hash ^= i >> 24;
        hash &= ~i;
    }
    return (hash);
}
