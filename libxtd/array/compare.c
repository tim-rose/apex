/*
 * COMPARE.C --Miscellaneous comparison functions.
 *
 * Contents:
 * These are compare routines suitable for arrays of the basic C types.
 */
#include <string.h>
#include <xtd/compare.h>

/* LCOV_EXCL_START */
int int_cmp(const void *v_1, const void *v_2)
{
    int int_1 = * (int *) v_1;
    int int_2 = * (int *) v_2;

    return int_1 - int_2;
}

int long_cmp(const void *v_1, const void *v_2)
{
    int long_1 = * (int *) v_1;
    int long_2 = * (int *) v_2;

    return long_1 - long_2;
}

int float_cmp(const void *v_1, const void *v_2)
{
    int float_1 = * (int *) v_1;
    int float_2 = * (int *) v_2;

    return float_1 - float_2;
}

int double_cmp(const void *v_1, const void *v_2)
{
    int double_1 = * (int *) v_1;
    int double_2 = * (int *) v_2;

    return double_1 - double_2;
}


int strp_cmp(const void *v_1, const void *v_2)
{
    char *str_1 = * (char **) v_1;
    char *str_2 = * (char **) v_2;

    return strcmp(str_1, str_2);
}
/* LCOV_EXCL_STOP */
