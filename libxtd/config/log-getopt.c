/*
 * LOG-GETOPT.C --Routines for setting log behaviour from command line options.
 *
 * Contents:
 * opt_log_inherit[] --special value string for opt_log_level.
 * opt_log_level()   --Set the system logging level via opts.
 */
#include <xtd.h>                       /* Windows_NT requires this before system headers */
#include <xtd/config.h>
#include <xtd/log.h>
#include <xtd/estring.h>

/*
 * opt_log_inherit[] --special value string for opt_log_level.
 */
char opt_log_inherit[] = "inherit from LOG_LEVEL";

/*
 * opt_log_level() --Set the system logging level via opts.
 *
 * Parameters:
 * name     --one of "log-level", "verbose", "quiet", "debug"
 * value    --(if name == "log-level") specifies the priority
 * unused   --an unused value required by the OptionProc's signature
 *
 * Remarks:
 * This is a callback function for the config/option system that sets
 * the logging level.  Most of the invocations are for boolean "flags"
 * (e.g. "--verbose", "--quiet", etc.), in which case the value field
 * is unused.  However, the nature of the config code is such that it
 * will be called with a "default" value too, and that default is
 * arranged to be the special string variable opt_log_inherit, so it
 * can be safely ignored (see CONFIG_LOG).
 */
int opt_log_level(const char *name, const char *value, void *UNUSED(unused))
{
    LogConfig c = *log_config(NULL);
    const char *priority = name;
    int status = 1;

    if (strcmp(name, "log-level") == 0)
    {
        priority = value;
        if (priority == opt_log_inherit)
        {
            return 1;                  /* ignore "default" value. */
        }
    }

    if (vstrmatch(priority, "quiet", "notice", (char *) NULL) >= 0)
    {
        c.threshold_priority = MAX(LOG_NOTICE, c.threshold_priority);
    }
    else if (vstrmatch(priority, "verbose", "info", (char *) NULL) >= 0)
    {
        c.threshold_priority = MAX(LOG_INFO, c.threshold_priority);
    }
    else if (strcmp(priority, "debug") == 0)
    {
        c.threshold_priority = MAX(LOG_DEBUG, c.threshold_priority);
    }
    else
    {
        err("unrecognised logging priority \"%s\"", priority);
        status = 0;
    }
    log_config(&c);
    return status;
}
