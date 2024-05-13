/*
 * SYSLOG.C --A log-handler that writes log messages to the syslog service.
 *
 * Contents:
 * log_syslog() --Output handler that logs a message to syslog.
 *
 * Remarks:
 * The handler opens and initialises the syslog connection the first
 * time it is called, but never closes it.
 */
#include <apex/log.h>

static int initialised;

/*
 * log_syslog() --Output handler that logs a message to syslog.
 */
int log_syslog(const LogConfig * config, const LogContext * caller,
               int sys_errno, size_t priority, const char *fmt, va_list args)
{
    int n;
    char text[LOG_LINE_MAX];

    if (!initialised)
    {
        openlog(config->identity, LOG_PID, (int) config->facility);
        setlogmask(LOG_UPTO(config->threshold_priority));
        initialised = 1;
    }

    n = log_vsprintf(caller, text, NEL(text), sys_errno, priority, fmt, args);

    if (n > 0)
    {
        syslog(priority, text);
    }
    return n;
}
