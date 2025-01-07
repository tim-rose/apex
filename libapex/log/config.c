/*
 * CONFIG.C --Configuration facilities for the logging facility.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * log_config[]  --Current log configuration state.
 * log_getenv_() --Load the logging parameters from the environment.
 * log_config()  --Set the current logging parameters, return the old ones..
 *
 * Remarks:
 * The log system configures its behaviour based on the following
 * environment variables:
 *
 * * LOG_IDENT  --the syslog identity to use for logging
 * * LOG_FACILITY --the syslog facility to use for logging
 * * LOG_LEVEL  --the syslog priority threshold
 * * LOG_OUTPUT --the log output handler
 * * LOG_TIMESTAMP  --a strftime(3) timestamp to prefix the message
 * * LOG_COLORS --ANSI style specifications for priorities (log_stderr).
 *
 * Note that the LOG_COLORS handling is special enough that it's been
 * isolated to its own module.
 */
#include <apex.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <stdio.h>

#include <apex/log.h>
#include <apex/sysenum.h>
#include <apex/estring.h>
#include <apex/strparse.h>

/*
 * log_config[] --Current log configuration state.
 *
 * Remarks:
 * log_state.output is set by log_handler(); FWIW, the default
 * implementation sets it to either log_stderr() or log_syslog().
 */
static LogConfig log_state = {
    .threshold_priority = LOG_NOTICE,  /* default values */
    .facility = LOG_USER,
};

static const LogConfig *config = NULL; /* Yow! are we initialised yet!!? */

/*
 * log_getenv_() --Load the logging parameters from the environment.
 *
 * Remarks:
 * This routine assumes that the values returned by getenv() are stable,
 * and that it's OK to use/store them (rather than strdup() the string).
 * It's all fun and games until someone's eye gets poked out with a putenv().
 */
static LogConfig *log_getenv_(LogConfig * conf)
{
    const char *value;
    SysEnum *e;

    if ((value = getenv("LOG_IDENT")) != NULL)
    {
        conf->identity = value;
    }
    if ((value = getenv("LOG_FACILITY")) != NULL
        && (e = sysenum_find_name(syslog_facility, value)) != NULL)
    {
        conf->facility = (size_t) e->value;
    }
    if ((value = getenv("LOG_LEVEL")) != NULL)
    {
        unsigned int priority;

        if (str_uint(value, &priority) && priority <= LOG_DEBUG)
        {                              /* priority specified numerically */
            conf->threshold_priority = priority;
        }
        else if ((e = sysenum_find_name(syslog_priority, value)) != NULL)
        {                              /* priority specified by keyword */
            conf->threshold_priority = (size_t) e->value;
        }
    }
    conf->output = log_handler(getenv("LOG_OUTPUT"));
    if ((value = getenv("LOG_TIMESTAMP")) != NULL)
    {
        conf->timestamp = value;
    }
    return conf;
}

/*
 * log_config() --Set the current logging parameters, return the old ones..
 *
 * Parameters:
 * new_config  --the new parameters
 *
 * Returns: (LogConfig *)
 * The old parameters.
 */
const LogConfig *log_config(const LogConfig * new_config)
{
    if (new_config != NULL)
    {
        static LogConfig old_config;

        old_config = log_state;
        log_state = *new_config;
        config = &log_state;
        return &old_config;
    }
    if (config == NULL)
    {                                  /* first time: load from environment */
        config = log_config(log_getenv_(&log_state));
    }
    return config;
}
