/*
 * ENUM.C --Enum utilities.
 *
 * Contents:
 * enum_cmp()   --Compare two Enums for sorting.
 * str_enum()   --Set an enum opts value.
 * enum_value() --Return the value for an Enum name.
 * enum_name()  --Return the name of an Enum value.
 *
 * Remarks:
 * Just some lookup convenience functions.
 * These rely/assume that the Enum array is well/NULL terminated.
 *
 */
#include <xtd.h>                       /* Windows_NT requires this before system headers */

#include <estring.h>
#include <symbol.h>

/*
 * enum_cmp() --Compare two Enums for sorting.
 */
int enum_cmp(const Enum * a, const Enum * b)
{
    return strcmp(a->name, b->name);
}

/*
 * str_enum() --Set an enum opts value.
 *
 * Parameters:
 * name  --the string option to be parsed as an enum name
 * n_item, item     --the enum items to match against "opt"
 * valp    --returns the selected item's value
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * Although the n_items parameter must be specified, note that this
 * code will bail at the first "empty" item slot anyway, so it's OK to
 * supply a "sufficiently large" value, as long as there's an empty
 * slot at the end.
 */
int str_enum(const char *opt, size_t n_items, Enum item[], int *valp)
{
    for (size_t i = 0; i < n_items; ++i)
    {
        if (item[i].name == NULL)
        {
            break;                     /* whoops: end of enum list */
        }
        if (strcmp(opt, item[i].name) == 0)
        {
            *valp = item[i].value;
            return 1;
        }
    }
    return 0;
}

/*
 * enum_value() --Return the value for an Enum name.
 */
int enum_value(const EnumPtr item, const char *name)
{
    int value = -1;

    str_enum(name, 1000, item, &value);
    return value;
}

/*
 * enum_name() --Return the name of an Enum value.
 */
const char *enum_name(EnumPtr item, int value)
{
    for (size_t i = 0; ; ++i)
    {
        if (item[i].name == NULL)
        {
            return NULL;                /* failure: end of list */
        }
        if (item[i].value == value)
        {
            return item[i].name;        /* success: found (first) value */
        }
    }
    return NULL;                        /* bonus failure: not reached! */
}
