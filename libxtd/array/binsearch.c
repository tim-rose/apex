/*
 * BINSEARCH.C --An improved implementation of libc's bsearch().
 *
 * Contents:
 * binsearch() --Search a sorted table using binary chop.
 *
 */
#include <xtd/binsearch.h>

/*
 * binsearch() --Search a sorted table using binary chop.
 *
 * key  --pointer to the key value to be found
 * base --the base of the array
 * n_elements  --the number of elements in the array
 * size  --the size of each element
 * compare   --a function to comapare elements
 * status   --returns the status of the final comparison.
 *
 * Returns: (int)
 * the offset in the table where the key value is, or should be inserted.
 *
 * Remarks:
 * This is an "improved" version of bsearch(); it wins over the former
 * in that it always returns a useful position in the table, and
 * returns (in the additonal argument "status") the "meaning" of the
 * returned position.
 *
 * Note: binsearch() will "not" work correctly if there are duplicates;
 * in particular, it may match to a random duplicate, or match to
 * past the end of the array.  You have been warned.
 */
int binsearch(void *key, void *base, int n_elements, int size,
              CompareProc compare, bool *status)
{
    int mid;
    int low, high, cmp;

    *status = false;                   /* status == not found */
    low = 0;
    high = n_elements - 1;
    cmp = mid = 0;
    while (low <= high)
    {                                  /* get midpoint */
        mid = (int) (low + high) / 2;
        cmp = (*compare) (key, (char *) base + mid * size);
        if (cmp < 0)                   /* select lower partition */
        {
            high = mid - 1;
        }
        else if (cmp > 0)              /* select higher partition */
        {
            low = mid + 1;
        }
        else
        {                              /* (cmp == 0): bingo! */
            *status = true;            /* status == found */
            return mid;
        }
    }
    if (cmp > 0)                       /* not found: return insertion slot */
    {
        return mid + 1;
    }
    return mid;
}
