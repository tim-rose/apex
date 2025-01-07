/*
 * LSEARCH.C --A simple linear search.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * lsearch() --Search a sorted table using .
 *
 * Remarks:
 * This routine has the same API as libc's bsearch().
 *
 */
#include <apex/search.h>

/*
 * lsearch() --Linear search an unsorted table.
 *
 * key  --pointer to the key value to be found
 * base --the base of the array
 * n_elements  --the number of elements in the array
 * size  --the size of each element
 * compare   --a function to comapare elements
 * status   --returns the status of the final comparison.
 *
 * Returns: (int)
 * Success: the slot of the found element; Failure: n_elements.
 */
int lsearch(void *key, void *base, size_t n_elements, size_t size, CompareProc
            compare, bool *status)
{
    char *item = base;

    for (size_t i = 0; i < n_elements; ++i, item += size)
    {
        if (compare(key, item) == 0)
        {
            *status = true;
            return i;                  /* success: key found in slot i */
        }
    }
    *status = false;
    return -1;                         /* failure: key not found */
}
