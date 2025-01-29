/*
 * COMPARE.C --Miscellaneous comparison functions.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * These are compare routines suitable for arrays of the basic C types.
 */
#include <string.h>
#include <apex/compare.h>

int array_cmp(const void *v_1, const void *v_2, 
    size_t n_items, size_t item_size, CompareProc cmp)
{
    char *c1 = (char *) v_1;
    char *c2 = (char *) v_2;
    char *end = c1 + n_items*item_size; 
    int status = 0;

    for (; c1 < end; c1 += item_size, c2 += item_size) 
    {
        status = cmp(c1, c2);
        if (status != 0)
        {
            break;
        }
    }
    return status;
}

int char_cmp(const void *const v_1, const void *const v_2)
{
    char char_1 = *(char *) v_1;
    char char_2 = *(char *) v_2;

    return char_1 - char_2;             /* no chance of overflow */
}

/* TODO: short_cmp() */

int int_cmp(const void *const v_1, const void *const v_2)
{
    int int_1 = *(int *) v_1;
    int int_2 = *(int *) v_2;

    if (int_1 > int_2)
    {
        return 1;                      /* v_1 is greater */
    }
    else if (int_1 < int_2)
    {
        return -1;                     /* v_2 is greater */
    }
    else
    {
        return 0;                      /* equal */
    }
}

int long_cmp(const void *const v_1, const void *const v_2)
{
    long long_1 = *(long *) v_1;
    long long_2 = *(long *) v_2;

    if (long_1 > long_2)
    {
        return 1;                      /* v_1 is greater */
    }
    else if (long_1 < long_2)
    {
        return -1;                     /* v_2 is greater */
    }
    else
    {
        return 0;                      /* equal */
    }
}

int float_cmp(const void *const v_1, const void *const v_2)
{
    float float_1 = *(float *) v_1;
    float float_2 = *(float *) v_2;

    if (float_1 > float_2)
    {
        return 1;                      /* v_1 is greater */
    }
    else if (float_1 < float_2)
    {
        return -1;                     /* v_2 is greater */
    }
    else
    {
        return 0;                      /* equal (but dodgy!) */
    }
}

int double_cmp(const void *const v_1, const void *const v_2)
{
    double double_1 = *(double *) v_1;
    double double_2 = *(double *) v_2;

    if (double_1 > double_2)
    {
        return 1;                      /* v_1 is greater */
    }
    else if (double_1 < double_2)
    {
        return -1;                     /* v_2 is greater */
    }
    else
    {
        return 0;                      /* equal (but dodgy!) */
    }
}


int strp_cmp(const void *const v_1, const void *const v_2)
{
    const char *str_1 = *(char **) v_1;
    const char *str_2 = *(char **) v_2;

    return strcmp(str_1, str_2);
}
