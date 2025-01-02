/*
 * SYSENUM.H --Definitions for some syslog variables.
 */
#ifndef APEX_SYSENUM_H
#define APEX_SYSENUM_H

#include <apex.h>

#include <stdarg.h>
#include SYSLOG
#include <errno.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct SysEnum           /* an alias for syslog CODE */
    {
        const char *name;
        int value;
    } SysEnum, *SysEnumPtr;

    extern SysEnumPtr syslog_facility; /* list of syslog facilities */
    extern SysEnumPtr syslog_priority; /* list of syslog priorities */

    SysEnumPtr sysenum_find_number(SysEnumPtr list, int number);
    SysEnumPtr sysenum_find_name(SysEnumPtr list, const char *name);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_SYSENUM_H */
