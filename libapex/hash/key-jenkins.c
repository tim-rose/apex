#include <apex/hash.h>
/*
 * hash_key_jenkins() --Bob Jenkins' hash.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * See Also:
 * http://burtleburtle.net/bob/hash/doobs.html
 */
unsigned long hash_key_jenkins(char *data)
{
    unsigned int hash;

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
