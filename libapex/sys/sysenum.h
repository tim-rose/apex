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
    } SysEnum;

    extern SysEnum *syslog_facility; /* list of syslog facilities */
    extern SysEnum *syslog_priority; /* list of syslog priorities */

    SysEnum *sysenum_find_number(SysEnum *list, int number);
    SysEnum *sysenum_find_name(SysEnum *list, const char *name);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_SYSENUM_H */
