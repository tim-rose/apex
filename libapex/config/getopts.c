/*
 * GETOPTS.C --Option-parsing/processing utilities.
 *
 * Contents:
 * getopts() --A stdarg variant of getopt().
 */
#include <apex.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>

#include <apex/getopts.h>
#include <apex/estring.h>

/*
 * OptionAddress --a union for holding the various option value addresses.
 */
typedef union OptionAddress
{
    char **str_addr;                   /* address of a string */
    int *int_addr;                     /* address of an (int) flag */
} OptionAddress;

/*
 * getopts() --A stdarg variant of getopt().
 *
 * Parameters:
 * argc     --No. of arguments (as passed to main())
 * argv     --argument vector (as passed to main())
 * opts     --string describing allowable options
 * arg...   --a list of pointers to write the values to.
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Discussion:
 * This is a convenience routine to handle the simple case of
 * a multitude of optional, once-off, non-conflicting options.
 *
 * If an option appears more than once, the subsequent instance
 * will overwrite any prior. If two options are mutually exclusive,
 * this must be checked after getopts() has returned.
 *
 * getopts() puts a pointer to the option value into *argn if it expects
 * one, or the value 1 otherwise. Option variables should therefore be
 * (addresses of) int or (char *), as appropriate.
 *
 * It is up to the caller to put appropriate sentinel values into
 * these variables prior to calling getopts().
 */
int getopts(int argc, const char *argv[], const char *opts, ...)
{
    va_list ap;
    int i;
    OptionAddress opt_addr[128] = { {0} };
    const char *opt = opts;

    va_start(ap, opts);

    while ((i = *opt++))
    {                                  /* load up arg value array... */
        if (*opt == ':')
        {
            opt_addr[i].str_addr = va_arg(ap, char **);

            ++opt;
        }
        else
        {
            opt_addr[i].int_addr = va_arg(ap, int *);
        }
    }

    va_end(ap);

    while ((i = getopt(argc, (char **) argv, opts)))
    {
        switch (i)
        {
        case -1:
            return (1);                /* EOF reached */
        case '?':
            return (0);
        default:
            opt = strchr(opts, i) + 1;
            if (*opt == ':')
            {
                *opt_addr[i].str_addr = optarg;
            }
            else
            {
                *opt_addr[i].int_addr = 1;
            }
        }
    }
    return (0);
}
