/*
 * MEMSWAP.C --Basic memory swapping routines.
 *
 * Contents:
 * memswap()     --swap memory.
 * memswap_int() --swap integer-aligned memory.
 *
 * Remarks:
 * This implements a basic fallback for memswap().
 * Hopefully your stdlib implements something better.
 */
#include <apex.h>                       /* Windows_NT requires this before system headers */

#include <stdint.h>
#include <apex/estring.h>
#define INT_SWAP_THRESHOLD 4*sizeof(int)

/*
 * memswap() --swap memory.
 *
 * Parameters:
 * m1, m2 --the address of the memory cells to swap
 * n    --the number of bytes to swap
 *
 * Remarks:
 * This routine checks to see if it can use memswap_int().
 */
void memswap(void *m1, void *m2, size_t n)
{
    char *c1 = m1, *c2 = m2;

    if (n > INT_SWAP_THRESHOLD         /* big enough to bother */
        && n % sizeof(int) == 0        /* size is alignment compatible */
        && (uintptr_t) c1 % sizeof(int) == 0    /* addresses are aligned */
        && (uintptr_t) c2 % sizeof(int) == 0)
    {
        memswap_int(m1, m2, n / sizeof(int));
        return;
    }

    do
    {
        char ch = *c1;

        *c1++ = *c2;
        *c2++ = ch;
    } while (--n > 0);
}

/*
 * memswap_int() --swap integer-aligned memory.
 *
 * Parameters:
 * i1, i2   --the address of the memory cells to swap
 * n    --the number of integer-sized chunks to swap
 *
 * Remarks:
 * This is a slightly faster version of memswap that is suitable
 * for integer-aligned+sized data, a common occurance in practice.
 */
void memswap_int(int *i1, int *i2, size_t n)
{
    do
    {
        int tmp = *i1;
        *i1++ = *i2;
        *i2++ = tmp;
    } while (--n > 0);
}
