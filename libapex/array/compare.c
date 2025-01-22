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
