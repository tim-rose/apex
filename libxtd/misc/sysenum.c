/*
 * SYSENUM.C --Routines for handling syslog's priority+faciltiy enums.
 */

#include <stdlib.h>
#include <string.h>

/*
 * Defining SYSLOG_NAMES before including <syslog.h> creates
 * definitions for syslog's priorities and facilities.
 */
#define SYSLOG_NAMES

#include <sysenum.h>

/*
 * syslog_facility[] --list of syslog facility names/values as a SysEnum.
 * syslog_priority[] --list of syslog priority names/values as a SysEnum.
 *
 * Remarks:
 * These variables are initialised from the "official" values defined
 * in (usually) /usr/include/sys/syslog.h, but I'm aliasing them into
 * a "SysEnum" type (which they happen to naturally match in structure).
 */
SysEnumPtr syslog_facility = (SysEnumPtr) facilitynames;
SysEnumPtr syslog_priority = (SysEnumPtr) prioritynames;
/*
 * sysenum_find_name() --Find a value by name in an SysEnum list.
 *
 * REVISIT: adapted/duplicated from symbol.c.
 */
SysEnumPtr sysenum_find_name(SysEnumPtr list, const char *name)
{
    for (SysEnumPtr item = list; item->name != NULL; ++item)
    {
        if (strcmp(item->name, name) == 0)
        {
            return item;               /* success */
        }
    }
    return NULL;                       /* failure */
}

/*
 * sysenum_find_number() --Find a value by number in a SysEnum list.
 *
 * REVISIT: adapted/duplicated from symbol.c.
 */
SysEnumPtr sysenum_find_number(SysEnumPtr list, int number)
{
    for (SysEnumPtr item = list; item->name != NULL; ++item)
    {
        if (item->value == number)
        {
            return item;               /* success */
        }
    }
    return NULL;                       /* failure */
}
