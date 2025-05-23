/*
 * HANDLER.C --Functions for choosing a log output function.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * log_handler() --Choose a log output function.
 *
 * Remarks:
 * This module is part of the library, and will be linked in by
 * default, but if the user supplies their own log_handler(), that
 * will be used instead.
 */
#include <apex.h>                       /* Windows_NT requires this before system headers */
#include <string.h>
#include <apex/log.h>

typedef struct LogAlias
{
    const char *name;
    LogOutputProc proc;
} LogAlias;

LogAlias log_alias[] = {
    {"syslog", log_syslog},
    {"stderr", log_stderr},
    {NULL, NULL}
};

/*
 * log_handler() --Choose a log output function.
 *
 * Parameters:
 * name --the name of the output function
 *
 * Returns: (LogOutputProc)
 * A suitable log output function.
 *
 * Remarks:
 * This function must *always* return a valid function!
 */
LogOutputProc log_handler(const char *name)
{
    if (name != NULL)
    {
        for (LogAlias *alias = log_alias; alias->name != NULL; ++alias)
        {
            if (strcmp(name, alias->name) == 0)
            {
                return alias->proc;
            }
        }
    }
    return log_stderr;
}
