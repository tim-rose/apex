/*
 * LOG-DOMAIN.C --Per-domain filtering for logging.
 *
 * Contents:
 * domain_config[]      --the current domain logging state.
 * VA_DOMAIN_LOG()      --Boilerplate var-args processing and logging behaviour.
 * VA_DOMAIN_VOID_LOG() --Boilerplate var-args processing for void functions.
 * vlog_msg()           --Log a message with fmt, va_list arguments.
 * log_domain_init()    --Intialise the logging state from a domain specification.
 * log_domain_status()  --Return (and initialise, if needed) a domain's log status.
 * log_domain_msg()     --Conditionally log a message based on domain.
 *
 * Remarks:
 * This module extends the basic logging, which restricts output by
 * priority, with per-domain filtering.  The domain filtering is
 * controlled by a list of domain names, which is either interpreted as
 * a list of domains to include (excluding all others), or exclude (and
 * thereby include all others).
 *
 * The default configuration is to log everything, but this can be
 * explicitly overridden by initialising with an explicit list.
 */
#include <log-domain.h>
#include <estring.h>

static char all_domains[] = "*";
static char *default_domains[] = { all_domains, NULL };

typedef struct LogDomainConfig_t
{
    char **domains;                    /* list of domain names */
    LogDomainStatus mode;              /* logging status of domains in list */
} LogDomainConfig, *LogDomainConfigPtr;

/*
 * domain_config[] --the current domain logging state.
 */
LogDomainConfig domain_config = { default_domains, LOG_DOMAIN_PRINT };

/*
 * VA_DOMAIN_LOG() --Boilerplate var-args processing and logging behaviour.
 */
#define VA_DOMAIN_LOG(_log_domain, _log_priority, _log_errno, _log_fmt)       \
    LogDomainStatus status = log_domain_status(_log_domain);          \
    va_list args;                                             \
    if (status != LOG_DOMAIN_PRINT)                            \
    {                                                         \
        return 0;                                             \
    }                                                         \
    va_start(args, _log_fmt);                                 \
    status = vlog_msg(_log_priority, _log_errno, _log_fmt, args);  \
    va_end(args);

/*
 * VA_DOMAIN_VOID_LOG() --Boilerplate var-args processing for void functions.
 */
#define VA_DOMAIN_VOID_LOG(_log_domain, _log_priority, _log_errno, _log_fmt) \
    LogDomainStatus status = log_domain_status(_log_domain);          \
    va_list args;                                             \
    if (status != LOG_DOMAIN_PRINT)                            \
    {                                                         \
        return;                                               \
    }                                                         \
    va_start(args, _log_fmt);                                 \
    status = vlog_msg(_log_priority, _log_errno, _log_fmt, args);  \
    va_end(args);

/*
 * vlog_msg() --Log a message with fmt, va_list arguments.
 */
static inline int vlog_msg(size_t priority, int log_errno, const char *fmt,
                           va_list args)
{
    LogConfigPtr config = log_config(NULL);

    if (priority <= config->threshold_priority)
    {
        return config->output(config, NULL, log_errno, priority, fmt, args);
    }
    return 0;
}

/*
 * log_domain_init() --Intialise the logging state from a domain specification.
 *
 * Parameters:
 * domain_spec  --the domain logging spec (e.g. "foo,bar,blah") or NULL
 *
 * Remarks:
 * The default configuration is to log everything, but this function
 * allows the caller to customise the config with a specific string,
 * or the environment variable LOG_DOMAINS.
 *
 * The specification string has the format: "domain1,domain2,domain3..."
 * which specifies that only the listed domains are to be printed.  If
 * the specification starts with the character "!"
 * (e.g. "!this,that,other") the config is treated as excluding the
 * listed domains, and printing everything else.
 *
 * If domain_spec is NULL, this routine uses the environment
 * variable LOG_DOMAINS to define the domain_spec.
 *
 * If this routine is never called, all domains are logged.
 */
void log_domain_init(const char *domain_spec)
{
    domain_config.mode = LOG_DOMAIN_PRINT;

    if (domain_spec == NULL && (domain_spec = getenv("LOG_DOMAINS")) == NULL)
    {
        domain_config.domains = (char **) default_domains;
        return;
    }

    if (*domain_spec == '!')
    {                                  /* negated list */
        domain_config.mode = LOG_DOMAIN_SUPPRESS;
        ++domain_spec;
    }
    if (domain_config.domains != default_domains)
    {                                  /* release previous domain list */
        free_str_list(domain_config.domains);
    }
    domain_config.domains = new_str_list(domain_spec, ',');
}

/*
 * log_domain_status() --Return (and initialise, if needed) a domain's log status.
 *
 * Parameters:
 * domain --specifies the domain
 *
 * Return: (LogDomainStatus)
 * The logging status of this domain, possibly freshly set.
 *
 * Remarks:
 * If the domain is uninitialised, this routine scans for it in the
 * list of domains created by log_domain_init(), and updates the domain's
 * status.
 */
static LogDomainStatus log_domain_status(LogDomainPtr domain)
{
    if (domain->status == LOG_DOMAIN_UNDEFINED)
    {
        for (char **g = domain_config.domains; *g != NULL; ++g)
        {
            if (*g == all_domains || strcmp(domain->name, *g) == 0)
            {                          /* linear scan: assume a small list */
                return domain->status = domain_config.mode;
            }
        }
        domain->status =
            (domain_config.mode == LOG_DOMAIN_PRINT)
            ? LOG_DOMAIN_SUPPRESS : LOG_DOMAIN_PRINT;
    }
    return domain->status;
}

/*
 *  log_domain_msg() --Conditionally log a message based on domain.
 *
 *  Parameters:
 *  domain  --the domain this message "belongs" to
 *  priority    --the message priority
 *  fmt...  --printf-style argument list
 *
 *  Returns: (int)
 *  No. of characters written.?
 */
int log_domain_msg(LogDomainPtr domain, size_t priority, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, priority, 0, fmt);
    return status;                     /* declared in macro! */
}

int log_domain_emerg(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_EMERG, 0, fmt);
    return status;
}

int log_domain_alert(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_ALERT, 0, fmt);
    return status;
}

int log_domain_crit(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_CRIT, 0, fmt);
    return status;
}

int log_domain_err(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_ERR, 0, fmt);
    return status;
}

int log_domain_warning(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_WARNING, 0, fmt);
    return status;
}

int log_domain_notice(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_NOTICE, 0, fmt);
    return status;
}

int log_domain_info(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_INFO, 0, fmt);
    return status;
}

int log_domain_debug(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_DEBUG, 0, fmt);
    return status;
}


int log_domain_quit(LogDomainPtr domain, int exit_status, const char *fmt,
                    ...)
{
    VA_DOMAIN_LOG(domain, LOG_ERR, 0, fmt);
    exit(exit_status);
}

int log_domain_sys(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_LOG(domain, LOG_ERR, 1, fmt);
    return status;
}

void log_domain_sys_quit(LogDomainPtr domain, int exit_status,
                         const char *fmt, ...)
{
    VA_DOMAIN_VOID_LOG(domain, LOG_ERR, 1, fmt);
    exit(exit_status);
}

void log_domain_sys_abort(LogDomainPtr domain, const char *fmt, ...)
{
    VA_DOMAIN_VOID_LOG(domain, LOG_ERR, 1, fmt);
    abort();
    exit(1);
}
