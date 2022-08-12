/*
 * MESSAGE.C --Various convenience functions for generating a log message.
 *
 * Contents:
 * VA_LOG()        --Boilerplate var-args processing and logging behaviour.
 * log_sys()       --Nonfatal error related to system call.
 * log_sys_quit()  --Fatal error related to system call.
 * log_sys_abort() --Fatal error related to system call, with crash dump.
 * log_msg()       --Nonfatal error, specified priority.
 * log_quit()      --Fatal error unrelated to system call.
 * trace_msg()     --Output a message with caller context.
 * STD_LOG()       --Boilerplate/macro for eponymous syslog priority messages.
 */
#include <stdlib.h>
#include <stdio.h>
#include <xtd/log.h>

/*
 * VA_LOG() --Boilerplate var-args processing and logging behaviour.
 *
 * Remarks:
 * All the logging functions are defined in terms of this macro.  Now
 * you know why Bjarne invented templates.
 */
#define VA_LOG(_log_priority, _log_errno, _log_fmt)      \
    va_list args; \
    int status = 0; \
    LogConfigPtr config = log_config(NULL); \
    if (_log_priority <= config->threshold_priority) { \
        va_start(args, _log_fmt); \
        status = config->output(config, NULL, \
                                _log_errno, _log_priority, fmt, args); \
        va_end(args); \
    }

/*
 * log_sys() --Nonfatal error related to system call.
 */
int log_sys(size_t priority, const char *fmt, ...)
{
    VA_LOG(priority, errno, fmt);
    return status;
}

/*
 * log_sys_quit() --Fatal error related to system call.
 */
void log_sys_quit(int exit_status, const char *fmt, ...)
{
    VA_LOG(LOG_ERR, errno, fmt);
    exit(exit_status);
}

/*
 * log_sys_abort() --Fatal error related to system call, with crash dump.
 */
void log_sys_abort(const char *fmt, ...)
{
    VA_LOG(LOG_ERR, errno, fmt);
    abort();                           /* dump core and terminate */
    exit(1);                           /* shouldn't get here */
}

/*
 * log_msg() --Nonfatal error, specified priority.
 */
int log_msg(size_t priority, const char *fmt, ...)
{
    VA_LOG(priority, 0, fmt);
    return status;
}

/*
 * log_quit() --Fatal error unrelated to system call.
 */
int log_quit(int exit_status, const char *fmt, ...)
{
    VA_LOG(LOG_CRIT, 0, fmt);
    exit(exit_status);
}

/*
 * trace_msg() --Output a message with caller context.
 */
int trace_msg(const char *func, const char *file, int line, size_t priority,
              const char *fmt, ...)
{
    va_list args;
    int status = 0;
    LogConfigPtr config = log_config(NULL);
    LogContext caller_context = {
        .function = func,.file = file,.line = line
    };

    if (priority <= config->threshold_priority)
    {
        va_start(args, fmt);
        status = config->output(config, &caller_context,
                                0, priority, fmt, args);
        va_end(args);
    }
    return status;
}

/*
 * STD_LOG() --Boilerplate/macro for eponymous syslog priority messages.
 */
#define STD_LOG(_priority, _name) \
    int _name(const char *fmt, ...) \
    { VA_LOG(_priority, 0, fmt); return status; }

STD_LOG(LOG_EMERG, emerg)
STD_LOG(LOG_ALERT, alert)
STD_LOG(LOG_CRIT, crit)
STD_LOG(LOG_ERR, err)
STD_LOG(LOG_WARNING, warning)
STD_LOG(LOG_NOTICE, notice) STD_LOG(LOG_INFO, info)
#ifdef debug
#undef debug                           /* remove any macro definition */
#endif
    STD_LOG(LOG_DEBUG, debug)
