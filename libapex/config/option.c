/*
 * OPTION.C --Option-parsing/processing callbacks for config/getopt.
 *
 * Contents:
 * opt_len_()          --Count the number of items in a 0-terminated opts list.
 * compile_opts_()     --Construct a getopt() option string from an opts list.
 * compile_longopts_() --Construct a getopt_long() option string.
 * opt_find_getopt_()  --Find an opts definition by its getopt() character.
 * opt_getopts()       --Process a main-style arglist with some opts options.
 * opt_getopts_long()  --Process a main-style arglist with long options.
 * opt_defaults()      --Process the default spec.s for each opts item.
 * opt_usage()         --Print a usage message describing options to stderr.
 *
 * Remarks:
 * These routines are useful as OptionProcs for config processing.
 */
#include <apex.h>
#include <apex/log.h>
#include <apex/symbol.h>
#include <apex/estring.h>
#include <apex/strparse.h>
#include <apex/convert.h>
#include <apex/date.h>
#include <apex/config.h>


/*
 * opt_len_() --Count the number of items in a 0-terminated opts list.
 */
static size_t opt_len_(Option opts[])
{
    OptionPtr opt = opts;

    while (opt->name != NULL)
    {
        ++opt;
    }
    return (size_t) (opt - opts);
}

/*
 * compile_opts_() --Construct a getopt() option string from an opts list.
 *
 * Parameters:
 * opts   --specifies the list of opts definitions
 * getopt_buf  --specifies and returns the buffer containing the compiled opts
 *
 * Returns: (char *)
 * getopt_buf.
 */
static char *compile_opts_(Option opts[], char *getopt_buf)
{
    char *opt = getopt_buf;

    for (OptionPtr c = opts; c != NULL && c->name != NULL; ++c)
    {
        if (c->opt)
        {
            *opt++ = c->opt;
            if (c->value != NULL)
            {
                *opt++ = ':';
            }
        }
    }
    *opt++ = '\0';
    return getopt_buf;
}

/*
 * compile_longopts_() --Construct a getopt_long() option string.
 *
 * Parameters:
 * opts   --specifies the list of opts definitions
 *
 * Returns: (char *)
 * Success: a malloc'd option list; Failure: NULL.
 */
static struct option *compile_longopts_(Option opts[])
{
    struct option *long_opts = NEW(struct option, opt_len_(opts) + 1);

    if (long_opts != NULL)
    {
        struct option *l_opt = long_opts;

        for (OptionPtr opt = opts; opt->name != NULL; ++opt, ++l_opt)
        {
            l_opt->name = opt->name;
            l_opt->has_arg =
                (opt->value == NULL) ? no_argument : required_argument;
        }
    }
    return long_opts;
}

/*
 * opt_find_getopt_() --Find an opts definition by its getopt() character.
 *
 * Parameters:
 * opts   --specifies the list of opts definitions
 * ch       --specifies the getopt() character
 *
 * Returns: (OptionPtr)
 * Success: the opts definition; Failure: NULL.
 */
static OptionPtr opt_find_getopt_(Option opts[], int ch)
{
    for (OptionPtr opt = opts; opt != NULL && opt->name != NULL; ++opt)
    {
        if (opt->opt == ch)
        {
            return opt;                /* success */
        }
    }
    return NULL;                       /* failure */
}

/*
 * opt_getopts() --Process a main-style arglist with some opts options.
 *
 * Parameters:
 * argc     --count of the number of arguments to process
 * argv     --the arguments to process (e.g. from main(...))
 * opts   --specifies the list of opts definitions
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This routine will abort on the first failure, but will have run
 * the callbacks for everything that succeeded to that point, which
 * may have side effects.
 */
int opt_getopts(int argc, char *argv[], Option opts[])
{
    char getopt_buf[128];
    char *short_opts = compile_opts_(opts, getopt_buf);
    int ch;

    while ((ch = getopt(argc, (char **) argv, short_opts)) >= 0)
    {
        const char *value = NULL;
        OptionPtr opt;

        if (ch == '?')
        {
            return 0;                  /* failure: bad option */
        }
        if ((opt = opt_find_getopt_(opts, ch)) == NULL)
        {
            err("\"-%c\": unrecognised option", ch);
            return 0;                  /* failure: bad name */
        }
        if (opt->value != NULL)
        {
            value = optarg;
        }
        if (!opt->set)
        {
            if (opt->proc != NULL && !opt->proc(opt->name, value, opt->data))
            {
                err("\"-%c %s\": unrecognised value", ch,
                    (value != NULL) ? value : "");
                return 0;              /* failure: bad value */
            }
            opt->set = 1;
        }
    }
    return 1;
}

/*
 * opt_getopts_long() --Process a main-style arglist with long options.
 *
 * Parameters:
 * argc     --count of the number of arguments to process
 * argv     --the arguments to process (e.g. from main(...))
 * opts   --specifies the list of opts definitions
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int opt_getopts_long(int argc, char *argv[], Option opts[])
{
    char getopt_buf[128];
    char *short_opts = compile_opts_(opts, getopt_buf);
    struct option *long_opts = compile_longopts_(opts);
    int slot;
    int status = 1;
    int ch;

    if (long_opts == NULL)
    {
        return 0;
    }

    optind = 1;
#ifdef HAVE_OPTRESET
    optreset = 1;
#endif /* HAVE_OPTRESET */
    while ((ch = getopt_long(argc, (char **) argv,
                             short_opts, long_opts, &slot)) >= 0)
    {
        OptionPtr opt;

        if (ch == 0)
        {                              /* a long option was parsed */
            if (slot < 0)
            {
                status = 0;
                break;
            }
            opt = opts + slot;
        }
        else
        {                              /* a short option was parsed */
            if ((opt = opt_find_getopt_(opts, ch)) == NULL)
            {
                if (ch != '?')
                {
                    err("\"-%c\": unrecognised option", ch);
                }
                status = 0;
                break;
            }
        }
        const char *value = (opt->value == NULL) ? NULL : optarg;

        if (!opt->set)
        {
            if (opt->proc != NULL && !opt->proc(opt->name, value, opt->data))
            {
                if (ch == 0)
                {
                    err("failed to process option --%s \"%s\"", opt->name,
                        (value != NULL) ? value : "");
                    status = 0;
                }
                else
                {
                    err("failed to process option -%c \"%s\"", ch,
                        (value != NULL) ? value : "");
                    status = 0;
                }
                break;
            }
            opt->set = 1;
        }
    }
    /* apply against argv[] */
    free(long_opts);
    return status;
}

/*
 * opt_defaults() --Process the default spec.s for each opts item.
 *
 * Parameters:
 * opts   --specifies the list of opts definitions
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int opt_defaults(Option opts[])
{
    for (OptionPtr opt = opts; opt != NULL && opt->name != NULL; ++opt)
    {
        if (opt->value != NULL && !opt->set)
        {
            if (opt->proc != NULL
                && !opt->proc(opt->name, opt->value, opt->data))
            {
                err("%s: unrecognised default value \"%s\"",
                    opt->name, opt->value);
                return 0;              /* failure: bad value */
            }
            opt->set = 1;
        }
    }
    return 1;
}

/*
 * opt_usage() --Print a usage message describing options to stderr.
 *
 * Parameters:
 * prologue --specifies prologue text (e.g. top-level summary)
 * opts   --specifies the list of opts definitions
 * epilogue --specifies epilogue text (e.g. exit status, etc.)
 *
 * Remarks:
 * This routine is a convenience function for creating a simple
 * usage message that includes information about the opts options.
 * It outputs text of the form:
 *
 * Usage:
 *     <prologue>
 *
 * Options:
 *     -x <name> <x's doc string>
 *     -y        set the <name> flag
 *     (etc.)
 *
 * <epilogue>
 */
void opt_usage(const char *prologue, Option opts[], const char *epilogue)
{
    const char indent[] = "    ";

    fprintf(stderr, "Usage:\n%s%s\n\nOptions:\n", indent, prologue);

    for (OptionPtr opt = opts; opt != NULL && opt->name != NULL; ++opt)
    {
        const char *value_name = opt->value_name ? opt->value_name : "value";

        fprintf(stderr, "%s", indent);

        if (opt->opt != '\0')
        {
            if (opt->value != NULL)
            {
                fprintf(stderr, "-%c <%s>, ", opt->opt, value_name);
            }
            else
            {
                fprintf(stderr, "-%c, ", opt->opt);
            }
        }
        if (opt->value != NULL)
        {
            fprintf(stderr, "--%s=<%s>", opt->name, value_name);
        }
        else
        {
            fprintf(stderr, "--%s", opt->name);
        }
        fprintf(stderr, "\n");
        if (opt->doc != NULL)
        {
            fprintf(stderr, "\t%s\n", opt->doc);
        }
        else if (opt->opt)
        {
            fprintf(stderr, "\tset the %s flag\n", opt->name);
        }
        if (opt->value != NULL)
        {
            fprintf(stderr, "\t(default: %s)\n", opt->value);
        }
    }
    if (epilogue != NULL)
    {
        fprintf(stderr, "\n%s\n", epilogue);
    }
}
