/*
 * LOG-DOMAIN.H --Logging facilities defined for functional domains.
 *
 * Remarks:
 * This domain extends basic logging to provide domain-specific control
 * over logging output. The idea is that the domain struct holds the name
 * and printing state of this domain's messages.  The first time a log
 * routine is called, the domain is queried/initialised.
 *
 * The configuration controlling which domains are printed is changeable
 * by the user via log_domain_init().  By default all domains are printed.
 */
#ifndef LOG_DOMAIN_H
#define LOG_DOMAIN_H

#include <apex/log.h>
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef enum LogDomainStatus
    {
        LOG_DOMAIN_PRINT = 1,
        LOG_DOMAIN_UNDEFINED = 0,
        LOG_DOMAIN_SUPPRESS = -1
    } LogDomainStatus, *LogDomainStatusPtr;

    typedef struct LogDomain_t
    {
        const char *name;
        LogDomainStatus status;
    } LogDomain, *LogDomainPtr;

    void log_domain_init(const char *domain_spec);

    /*
     * These routines are simply "domain-enhanced" versions of the
     * base logging routines.
     */
    int log_domain_msg(LogDomainPtr domain, size_t priority, const char *fmt,
                       ...) PRINTF_ATTRIBUTE(3, 4);

    int log_domain_emerg(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_alert(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_crit(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_err(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_warning(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_notice(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_info(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_debug(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    int log_domain_quit(LogDomainPtr domain, int exit_status, const char *fmt, ...) PRINTF_ATTRIBUTE(3, 4); /* fatal proc */

    int log_domain_sys(LogDomainPtr domain, const char *fmt, ...)
        PRINTF_ATTRIBUTE(2, 3);
    void log_domain_sys_quit(LogDomainPtr domain, int exit_status,
                             const char *fmt, ...) PRINTF_ATTRIBUTE(3, 4);
    void log_domain_sys_abort(LogDomainPtr domain, const char *fmt,
                              ...) PRINTF_ATTRIBUTE(2, 3);

    /*
     * These routines provide log caller information.
     */
    int trace_domain_msg(LogDomainPtr domain, const char *func,
                         const char *file, int line, size_t priority,
                         const char *fmt, ...) PRINTF_ATTRIBUTE(6, 7);

#define trace_domain_emerg(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_EMERG, \
                    domain, fmt, ##__VA_ARGS__)
#define trace_domain_alert(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_ALERT, \
                    domain, fmt, ##__VA_ARGS__)
#define trace_domain_crit(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_CRIT, \
                    domain, fmt, ##__VA_ARGS__)
#define trace_domain_err(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_ERR, \
                    domain, fmt, ##__VA_ARGS__)
#define trace_domain_warning(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_WARNING, \
                    domain, fmt, ##__VA_ARGS__)
#define trace_domain_notice(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_NOTICE, \
                    domain, fmt, ##__VA_ARGS__)
#define trace_domain_info(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_INFO, \
                    domain, fmt, ##__VA_ARGS__)
#define trace_domain_debug(domain, fmt, ...) \
    trace_domain_msg(__func__, __FILE__, __LINE__, LOG_DEBUG, \
                    domain, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* LOG_DOMAIN_H */
