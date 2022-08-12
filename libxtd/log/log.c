/*
 * LOG.C --Logging facilities.
 *
 * Contents:
 * log_init()     --Initialise the logging system.
 * log_vsprintf() --Format a log message into a text buffer.
 *
 * Remarks:
 * This module is adapted/inspired from Stevens' excellent
 * examples/sample source from the book Unix Network Programming.  It
 * provides an assortment of convenience functions for logging
 * messages that models syslog's priorities and includes macros for
 * logging caller/trace information (i.e. file, line, function).
 *
 * It bears little resemblance to the original.
 *
 * References:
 * Unix Network Programming Vol1, 3rd Ed..  Stevens et al, Addison Wesley.
 */
#include <stdlib.h>
#include <stdio.h>
#include <xtd/log.h>
#include <xtd/sysenum.h>
#include <xtd/estring.h>

/*
 * log_init() --Initialise the logging system.
 *
 * Parameters:
 * identity --the syslog identity of this logging application.
 */
LogConfigPtr log_init(const char *identity)
{
    LogConfig log = *log_config(NULL);

    log.identity = identity;
    return log_config(&log);
}

/*
 * log_vsprintf() --Format a log message into a text buffer.
 *
 * Parameters:
 * config   --the current logging state
 * caller   --the caller context(func, file, line)
 * str  --the text buffer to write the message to
 * len  --the writable length of the text buffer
 * sys_errno    --specifies the system error (or 0)
 * priority --specifies the syslog priority
 * fmt, ... --the caller log message
 *
 * Returns: (int)
 * Success: The number of characters the message required; Failure: -1.
 *
 * Remarks:
 * This is a common formatting routine for use by the output
 * handlers. It outputs a prefix indicating the syslog priority, and
 * in addition to formatting the supplied printf()-style message
 * arguments, it conditionally includes caller context and system
 * error (i.e. errno) related text.
 *
 * Note that the return value is snprintf()-like: it will return the
 * number of characters in the message, even when that is larger than
 * the buffer used to store it (i.e. the return value can be greater
 * than len).
 */
int log_vsprintf(LogContextPtr caller,
                 char *str, size_t len,
                 int sys_errno, size_t priority,
                 const char *fmt, va_list args)
{
    SysEnumPtr sys_priority;
    char *start = str;
    char *end = str + len;
    int n;

    if (caller != NULL)
    {
        if (caller->function != NULL)
        {
            str += snprintf(str, MAX(end - str, 0), "%s:", caller->function);
        }
        str += snprintf(str, MAX(end - str, 0),
                        "%s:%d: ", STR_OR_NULL(caller->file), caller->line);
    }

    if ((sys_priority =
         sysenum_find_number(syslog_priority, priority)) != NULL)
    {                                  /* prefix with priority name */
        str += snprintf(str, MAX(end - str, 0), "%s: ", sys_priority->name);
    }
    /* format the caller-supplied message */

    if ((n = vsnprintf(str, MAX(end - str, 0), fmt, args)) >= 0)
    {
        str += n;
    }
    else
    {
        return -1;                     /* error: bad sprintf format */
    }

    if (sys_errno)
    {                                  /* append errno-related message */
        str += snprintf(str, MAX(end - str, 0), ": %s", strerror(sys_errno));
    }
    return str - start;
}
