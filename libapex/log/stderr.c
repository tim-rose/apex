/*
 * STDERR.C --A log-handler that writes log messages to stderr.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * fallback_colours[] --fallback values for tty colour styles.
 * init()             --Initialise stderr logging state.
 * log_stderr()       --Output handler that logs a message to stderr.
 *
 * Remarks:
 * The stderr handler prefixes the log message with a strftime()-specified
 * timestamp, and can conditionaly print the text with some cheezy
 * ANSI styles based on the priority.
 */
#include <stdio.h>
#include <time.h>
#include <unistd.h>                    /* isatty */

#include <apex/log.h>
#include <apex/sysenum.h>
#include <apex/estring.h>
#include <apex/strparse.h>

#ifdef __WINNT__
#define localtime_r(timep_, result_) localtime(timep_)
#endif /* __WINNT__ */

/*
 * fallback_colours[] --fallback values for tty colour styles.
 *
 * Remarks:
 * The colours are specified as a simple string in the format that
 * would be specified as an environment variable.
 */
static const char fallback_colours[] = "debug=36"   /* cyan */
    ":notice=1"                        /* bold */
    ":warning=1;33"                    /* bold + yellow */
    ":err=1;31"                        /* bold + red */
    ":crit=1;33;41"                    /* bold + yellow/red */
    ":alert=1;5;33;41"                 /* bold + flashing + yellow/red */
    ":emerg=1;5;37;41";                /* bold + flashing + white/red */

static char *log_colour[8];
static char **priority_colour;         /* initialised values */

/*
 * init() --Initialise stderr logging state.
 *
 * Remarks:
 * The main work here is building a set of ASCII escapes for styling
 * the output, but only if stderr is a real tty.
 * REVISIT: consider refactoring to avoid new/free_str_list.
 */
static char **init(void)
{
    priority_colour = log_colour;      /* mark initialised */

    if (!isatty(2))
    {
        return priority_colour;
    }

    const char *colour_spec;

    if ((colour_spec = getenv("LOG_COLORS")) == NULL)
    {
        colour_spec = fallback_colours;
    }

    char **list = new_str_list(colour_spec, ':');
    char **items = list;

    for (char *item = *items++; item != NULL; item = *items++)
    {
        SysEnum *priority;
        char name[10 + 1];             /* hardcoded to match sscanf() fmt */
        char priority_spec[20 + 1];

        if (sscanf(item, "%10[^=]=%20s", name, priority_spec) == 2
            && (priority = sysenum_find_name(syslog_priority, name)) != NULL)
        {
            log_colour[priority->value] = strdup(priority_spec);
        }
    }
    free_str_list(list);

    return priority_colour;
}

/*
 * log_stderr() --Output handler that logs a message to stderr.
 *
 * Remarks:
 * If stderr appears to be closed, this routine will automatically
 * reconfigure the logger to use syslog.  This is typical behaviour
 * if a program runs as a daemon.
 *
 * log_stderr() goes to some trouble to ensure that the message is
 * output with a single fputs(), and won't be comingled/corrupted by
 * other processes writing to the same effective file.  However, it
 * doesn't care so much about the colour codes. which are in separate
 * calls to fputs().
 */
int log_stderr(const LogConfig * config, const LogContext * caller,
               int sys_errno, size_t priority, const char *fmt, va_list args)
{
    char eol[] = "\n";
    char text[LOG_LINE_MAX + NEL(eol)];
    char *str = text;
    char *end = text + LOG_LINE_MAX;
    char **colour = priority_colour ? priority_colour : init();
    int n;
    int status = 0;

    if (config->timestamp != NULL)
    {
        time_t now = time(0);
        struct tm local_time;

        localtime_r(&now, &local_time);
        if ((n = strftime(str, (size_t) (end - str),
                          config->timestamp, &local_time)))
        {
            str += n;
            if (str < end)
            {
                *str++ = ' ';          /* implicit spacer. */
            }
        }
    }

    if (config->identity != NULL)
    {
        str += snprintf(str, (size_t) MAX(end - str, 0), "%s ", config->identity);
    }

    if ((n = log_vsprintf(caller, str, (size_t) MAX(end - str + 1, 0),
                          sys_errno, priority, fmt, args)) >= 0)
    {
        str += n;
    }
    else
    {
        return -1;
    }

    fflush(stdout);                    /* in case stdout == stderr */

    if (colour[priority])
    {                                  /* (hard-coded) SGR to set colours */
        status =
            fprintf(stderr, "\033[%sm%s\033[m\n",
                    priority_colour[priority], text);
    }
    else
    {
        status = fprintf(stderr, "%s\n", text);
    }
    if (status < 0)
    {                                  /* stderr failed: fallback to syslog! */
        LogConfig syslog_config = *config;

        syslog_config.output = log_syslog;
        config = log_config(&syslog_config);
        return log_syslog(config, caller, sys_errno, priority, fmt, args);
    }
    fflush(stderr);
    return str - text;
}
