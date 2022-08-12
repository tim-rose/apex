/*
 * OPTION.C --Option-parsing/processing callbacks for config/getopt.
 *
 * Contents:
 * opt_string()      --Parse a string option.
 * opt_bool()        --Parse a boolean/flag option.
 * opt_int()         --Parse an integer option.
 * opt_float()       --Parse a float option.
 * opt_double()      --Parse a double option.
 * opt_length()      --Parse and validate a length option.
 * opt_velocity()    --Parse and validate a velocity option.
 * opt_duration()    --Parse and validate a duration option.
 * opt_temperature() --Parse and validate a temperature option.
 * opt_timestamp()   --Parse and validate a timestamp option.
 * opt_input_file()  --Parse, validate an input file option.
 *
 * Remarks:
 * These routines are useful as OptionProcs for config processing.
 */
#include <xtd.h>
#include <xtd/log.h>
#include <xtd/symbol.h>
#include <xtd/estring.h>
#include <xtd/strparse.h>
#include <xtd/convert.h>
#include <xtd/date.h>
#include <xtd/config.h>

/*
 * opt_string() --Parse a string option.
 *
 * Parameters:
 * name     --the option name (unused)
 * text    --the variable text to be parsed (as a string)
 * data     --the address of a string to save the value to
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * The returned string is allocated via strdup().
 */
int opt_string(const char *UNUSED(name), const char *text, void *data)
{
    const char **str = (const char **) data;

    if (text)
    {
        if (*str != NULL)
        {                              /* discard previous value */
            free((void *) *str);
        }
        if ((*str = strdup(text)) == NULL)
        {
            return 0;
        }
    }
    return 1;
}

/*
 * opt_bool() --Parse a boolean/flag option.
 *
 * Parameters:
 * name     --the configuration variable name (unused)
 * text    --the configuration variable text (as a string)
 * data     --the address of an integer to save the boolean state
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * Note that this proc may be called with a NULL value, probably
 * as part of command-line configuration, in which case it will
 * simply set the (assumed int) data.
 */
int opt_bool(const char *UNUSED(name), const char *text, void *data)
{
    int *valp = (int *) data;
    static Enum bool_values[] = {
        {"1", 1},                      /* aliases for boolean values */
        {"true", 1},
        {"on", 1},
        {"yes", 1},
        {"ok", 1},
        {"0", 0},
        {"false", 0},
        {"off", 0},
        {"no", 0},
    };

    if (text != NULL)
    {
        return str_enum(text, NEL(bool_values), bool_values, valp);
    }
    *valp = 1;
    return 1;
}

/*
 * opt_int() --Parse an integer option.
 */
int opt_int(const char *UNUSED(name), const char *text, void *data)
{
    return str_int(text, (int *) data);
}

/*
 * opt_float() --Parse a float option.
 */
int opt_float(const char *UNUSED(name), const char *text, void *data)
{
    return str_float(text, (float *) data);
}

/*
 * opt_double() --Parse a double option.
 */
int opt_double(const char *UNUSED(name), const char *text, void *data)
{
    return str_double(text, (double *) data);
}

/*
 * opt_length() --Parse and validate a length option.
 */
int opt_length(const char *UNUSED(name), const char *text, void *data)
{
    return str_convert(text, (double *) data, convert_length);
}

/*
 * opt_velocity() --Parse and validate a velocity option.
 */
int opt_velocity(const char *UNUSED(name), const char *text, void *data)
{
    return str_convert(text, (double *) data, convert_velocity);
}

/*
 * opt_duration() --Parse and validate a duration option.
 */
int opt_duration(const char *UNUSED(name), const char *text, void *data)
{
    return str_convert(text, (double *) data, convert_duration);
}

/*
 * opt_temperature() --Parse and validate a temperature option.
 */
int opt_temperature(const char *UNUSED(name), const char *text, void *data)
{
    return str_convert(text, (double *) data, convert_temperature);
}

/*
 * opt_timestamp() --Parse and validate a timestamp option.
 *
 * FIXME: parse timestamp into a timeval struct.
 */
int opt_timestamp(const char *UNUSED(name), const char *text, void *data)
{
    struct tm t_tm = null_tm;
    const char *end = date_parse_timestamp(text, &t_tm, (time_t *) data);

    return (*end == '\0') ? 1 : 0;
}

/*
 * opt_input_file() --Parse, validate an input file option.
 */
int opt_input_file(const char *UNUSED(name), const char *file_name,
                   void *data)
{
    FILE *input_file_;

    if (strcmp(file_name, "stdin") == 0 || strcmp(file_name, "-") == 0)
    {
        input_file_ = stdin;
    }
    else if ((input_file_ = fopen(file_name, "r")) == NULL)
    {
        log_sys(LOG_ERR, "cannot open input file \"%s\"", file_name);
        return 0;                      /* failure: cannot open file */
    }
    debug("opening \"%s\" for reading", file_name);

    *(FILE **) data = input_file_;
    return 1;                          /* success */
}
