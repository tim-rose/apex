/*
 * COMPARE.C --Miscellaneous comparison functions.
 *
 * Contents:
 * TBD
 */
#include <string.h>
#include <compare.h>

int int_cmp(const void *v_1, const void *v_2)
{
    int int_1 = * (int *) v_1;
    int int_2 = * (int *) v_2;

    return int_1 - int_2;
}

int strp_cmp(const void *v_1, const void *v_2)
{
    char *str_1 = * (char **) v_1;
    char *str_2 = * (char **) v_2;

    return strcmp(str_1, str_2);
}
