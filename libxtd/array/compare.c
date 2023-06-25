/*
 * COMPARE.C --Miscellaneous comparison functions.
 *
 * Contents:
 * These are compare routines suitable for arrays of the basic C types.
 */
#include <string.h>
#include <xtd/compare.h>

int int_cmp(const void * const v_1, const void * const v_2)
{
    int int_1 = * (int *) v_1;
    int int_2 = * (int *) v_2;

    if (int_1 > int_2)
    {
        return 1;                       /* v_1 is greater */
    }
    else if (int_1 < int_2)
    {
        return -1;                      /* v_2 is greater */
    }
    else
    {
        return 0;                       /* equal */
    }
}

int long_cmp(const void * const v_1, const void * const v_2)
{
    int long_1 = * (long *) v_1;
    int long_2 = * (long *) v_2;

    if (long_1 > long_2)
    {
        return 1;                       /* v_1 is greater */
    }
    else if (long_1 < long_2)
    {
        return -1;                      /* v_2 is greater */
    }
    else
    {
        return 0;                       /* equal */
    }
}

int float_cmp(const void * const v_1, const void * const v_2)
{
    int float_1 = * (float *) v_1;
    int float_2 = * (float *) v_2;

    if (float_1 > float_2)
    {
        return 1;                       /* v_1 is greater */
    }
    else if (float_1 < float_2)
    {
        return -1;                      /* v_2 is greater */
    }
    else
    {
        return 0;                       /* equal (but dodgy!)*/
    }
}

int double_cmp(const void * const v_1, const void * const v_2)
{
    int double_1 = * (double *) v_1;
    int double_2 = * (double *) v_2;

    if (double_1 > double_2)
    {
        return 1;                       /* v_1 is greater */
    }
    else if (double_1 < double_2)
    {
        return -1;                      /* v_2 is greater */
    }
    else
    {
        return 0;                       /* equal (but dodgy!)*/
    }
}


int strp_cmp(const void * const v_1, const void * const v_2)
{
    char *str_1 = * (char **) v_1;
    char *str_2 = * (char **) v_2;

    return strcmp(str_1, str_2);
}
