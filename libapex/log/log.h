/*
 * LOG.H --Logging facilities.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 */
#ifndef APEX_LOG_H
#define APEX_LOG_H

#include <apex.h>                       /* Windows_NT requires this before system headers */
#include <stdarg.h>
#include SYSLOG
#include <errno.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    enum LogConsts
    {
        LOG_LINE_MAX = 250,            /* longer lines are truncated */
    };

    /*
     * These routines simply print/syslog the message.
     */
    int log_msg(size_t priority, const char *fmt, ...) PRINTF_ATTRIBUTE(2, 3);

    int emerg(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int alert(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int crit(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int err(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int warning(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int notice(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int info(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int debug(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);
    int log_quit(int exit_status, const char *fmt, ...) PRINTF_ATTRIBUTE(2, 3); /* fatal proc */

    /*
     * These routines append the errno-related message to the output,
     * and log at LOG_ERROR severity.
     */
    int log_sys(size_t priority, const char *fmt, ...) PRINTF_ATTRIBUTE(2, 3);
    void log_sys_quit(int exit_status, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    void log_sys_abort(const char *fmt, ...) PRINTF_ATTRIBUTE(1, 2);

    /*
     * These routines provide log caller information.
     */
    int trace_msg(const char *func, const char *file, int line,
                  size_t priority, const char *fmt, ...) PRINTF_ATTRIBUTE(5,
                                                                          6);

#define trace_emerg(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_EMERG, fmt, ##__VA_ARGS__)
#define trace_alert(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_ALERT, fmt, ##__VA_ARGS__)
#define trace_crit(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_CRIT, fmt, ##__VA_ARGS__)
#define trace_err(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_ERR, fmt, ##__VA_ARGS__)
#define trace_warning(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_WARNING, fmt, ##__VA_ARGS__)
#define trace_notice(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_NOTICE, fmt, ##__VA_ARGS__)
#define trace_info(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_INFO, fmt, ##__VA_ARGS__)
#define trace_debug(fmt, ...) \
    trace_msg(__func__, __FILE__, __LINE__, LOG_DEBUG, fmt, ##__VA_ARGS__)

    typedef struct LogConfig LogConfig;
    typedef struct LogContext LogContext;

    /*
     * LogOutputProc() --Signature for a function that emits a log message.
     *
     * Parameters:
     * config   --the current logging configuration
     * caller   --the caller's context (or NULL)
     * sys_errno --the system errno (or 0)
     * priority --the priority of the message
     * fmt,...  --printf-style arguments
     *
     * Remarks:
     * The actual output of a log message is handled by a function
     * with this signature.  The log module defines two useful examples:
     *
     *  * log_stderr --log the message to the stderr stream with a timestamp
     *  * log_syslog --log the message to the syslog service.
     *
     *  Custom handlers can be defined by log_config(), log_handler().
     */
    typedef int (*LogOutputProc)(const LogConfig * config,
                                 const LogContext * caller,
                                 int sys_errno, size_t priority,
                                 const char *fmt, va_list args);
    /*
     * Built-in log handlers...
     */
    int log_stderr(const LogConfig * config, const LogContext * caller,
                   int sys_errno,
                   size_t priority, const char *fmt, va_list args)
        PRINTF_ATTRIBUTE(5, 0);
    int log_syslog(const LogConfig * config, const LogContext * caller,
                   int sys_errno,
                   size_t priority, const char *fmt, va_list args)
        PRINTF_ATTRIBUTE(5, 0);

    const LogConfig *log_init(const char *identity);
    LogOutputProc log_handler(const char *name);
    const LogConfig *log_config(const LogConfig * new_config);
    int log_vsprintf(const LogContext * caller,
                     char *str, size_t len,
                     int sys_errno, size_t priority,
                     const char *fmt, va_list args);

    /*
     * LogConfig --global state for the logging system.
     */
    struct LogConfig
    {
        size_t threshold_priority;     /* active logging level */
        const char *identity;          /* syslog identity */
        size_t facility;               /* syslog facility */
        const char *timestamp;         /* strftime timestamp spec */
        LogOutputProc output;          /* message handler */
    };

    /*
     * LogContext --Caller context for "trace" log messages.
     */
    struct LogContext
    {
        const char *function;          /* __func__ */
        const char *file;              /* __FILE__ */
        int line;                      /* __LINE__ */
    };
#ifdef __cplusplus
}
#endif                                 /* C++ */

/*
 * NDEBUG --simulate assert()'s behaviour.
 *
 * Remarks:
 * If NDEBUG is defined, the "debug" log functions are expanded to NOPs,
 * and likewise the assert macro.  Note that we hijack the well-known
 * assert macro to integrate it with this logging framework.
 */
#ifdef NDEBUG
#define debug(fmt, ...)
#define trace_debug(__func__, __FILE__, __LINE__, fmt, ...)
#define assert(test) (void) 0
#else
#define assert(test) \
    (void)((test) || (trace_crit("failed assertion: %s", #test), abort(), 1));
#endif                                 /* NDEBUG */

#endif                                 /* APEX_LOG_H */
