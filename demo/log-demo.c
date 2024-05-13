/*
 * LOG-DEMO.C --test the logging stuff
 *
 * Contents:
 * main() --Log some message
 *
 * Remarks:
 */
/* LCOV_EXCL_START */

#include <stdio.h>
#include <stdlib.h>

#include <apex.h>
#include <apex/getopts.h>
#include <apex/log-domain.h>

LogDomain lg = { "demo", 0 };

/*
 * main() --Log some message
 */
int main(int UNUSED(argc), char *UNUSED(argv[]))
{
    log_init("log-echo");              /* declare application name to logger */
    log_domain_init(NULL);             /* initialise list of logged domains */
    debug("initialisation complete");
    trace_warning("some low-level detail...");
    log_domain_msg(&lg, LOG_NOTICE, "(%s): something important happened",
                   lg.name);
    log_domain_info(&lg, "(%s): something minor happens...", lg.name);
    errno = 1;

    exit(0);
}

/* LCOV_EXCL_STOP */
