/*
 * CONVERT.C --Unit conversion framework.
 *
 * Contents:
 * convert_velocity[]   --Convert velocity/speed to SI: m/s.
 * convert_length[]     --Convert length to SI: metres.
 * convert_duration[]   --Convert time/duration to SI: seconds.
 * convert_temperature[] --Convert temperature to SI(ish): Celsius.
 * convert_mass[]       --Convert mass to SI: kg.
 * str_convert()        --Parse a string as a (double) and convert to SI units.
 * str_convertn()       --Parse a string and convert to some units.
 * scale()              --Simple scaling conversion function.
 * linear()             --Simple linear (y = mx + c) conversion function.
 *
 * Remarks:
 * This module implements a simple API for scaling values into SI
 * units.  Although it doesn't handle converting back, there is enough
 * information in the data to do so.  Maybe one day.  I'm not trying
 * to re-implement units(1), this is intended for parsing command line
 * and configuration file values.
 */

#include <stdlib.h>
#include <errno.h>

#include <convert.h>
#include <estring.h>

static double kph_factor = 1 / 3.6;
static double mph_factor = 1.609344 / 3.6;
static double inch_factor = 0.0254;
static double foot_factor = 0.3048;
static double mile_factor = 1.609344 * 1000;

static double unity_factor = 1;
static double UNUSED(M_factor) = 1000000.0;
static double k_factor = 1000.0;
static double c_factor = 1 / 100.0;
static double UNUSED(d_factor) = 1 / 10.0;
static double m_factor = 1 / 1000.0;
static double u_factor = 1 / 1000000.0;

/*
 * convert_velocity[] --Convert velocity/speed to SI: m/s.
 */
Conversion convert_velocity[] = {
    {"kph", (ConvertProc) scale, (void *) &kph_factor},
    {"km/h", (ConvertProc) scale, (void *) &kph_factor},
    {"mph", (ConvertProc) scale, (void *) &mph_factor},
    {"ft/s", (ConvertProc) scale, (void *) &foot_factor},
    {"inch/s", (ConvertProc) scale, (void *) &inch_factor},
    {"m/s", (ConvertProc) scale, (void *) &unity_factor},
    {NULL, NULL, NULL}
};

/*
 * convert_length[] --Convert length to SI: metres.
 */
Conversion convert_length[] = {
    {"m", (ConvertProc) scale, (void *) &unity_factor},
    {"metre", (ConvertProc) scale, (void *) &unity_factor},
    {"metres", (ConvertProc) scale, (void *) &unity_factor},
    {"cm", (ConvertProc) scale, (void *) &c_factor},
    {"mm", (ConvertProc) scale, (void *) &m_factor},
    {"km", (ConvertProc) scale, (void *) &k_factor},
    {"in", (ConvertProc) scale, (void *) &inch_factor},
    {"inch", (ConvertProc) scale, (void *) &inch_factor},
    {"inches", (ConvertProc) scale, (void *) &inch_factor},
    {"\"", (ConvertProc) scale, (void *) &inch_factor},
    {"ft", (ConvertProc) scale, (void *) &foot_factor},
    {"foot", (ConvertProc) scale, (void *) &foot_factor},
    {"feet", (ConvertProc) scale, (void *) &foot_factor},
    {"'", (ConvertProc) scale, (void *) &foot_factor},
    {"mi", (ConvertProc) scale, (void *) &mile_factor},
    {"mile", (ConvertProc) scale, (void *) &mile_factor},
    {NULL, NULL, NULL}
};

/*
 * convert_duration[] --Convert time/duration to SI: seconds.
 *
 * REVISIT: store duration as a timeval.
 */
Conversion convert_duration[] = {
    {"s", (ConvertProc) scale, (void *) &unity_factor},
    {"ms", (ConvertProc) scale, (void *) &m_factor},
    {"us", (ConvertProc) scale, (void *) &u_factor},
    {NULL, NULL, NULL}
};

/*
 * convert_temperature[] --Convert temperature to SI(ish): Celsius.
 *
 * Strictly speaking Kelvin is the SI unit, but Celsius is
 * more common, even in scientific contexts.
 */
static LinearConvertArg fahrenheit_arg = {
    .m = 5.0 / 9.0,.c = -32.0 * 5.0 / 9.0
};

static LinearConvertArg kelvin_arg = {
    .m = 1.0,.c = -273.15
};

Conversion convert_temperature[] = {
    {"C", (ConvertProc) scale, (void *) &unity_factor},
    {"K", (ConvertProc) linear, (void *) &kelvin_arg},
    {"F", (ConvertProc) linear, (void *) &fahrenheit_arg},
    {NULL, NULL, NULL}
};

/*
 * convert_mass[] --Convert mass to SI: kg.
 */
Conversion convert_mass[] = {
    {"kg", (ConvertProc) scale, (void *) &unity_factor},
    {"g", (ConvertProc) scale, (void *) &k_factor},
    /* TODO: add more alternative mass units */
    {NULL, NULL, NULL}
};

/*
 * str_convert() --Parse a string as a (double) and convert to SI units.
 *
 * Parameters:
 * opt  --the string to convert
 * value_ptr --returns the parsed and converted value
 * conversions --a NULL-terminated list of conversions to try
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int str_convert(const char *opt, double *value_ptr, Conversion conversions[])
{
    double value;
    char *end;
    ConversionPtr conversion;

    if (opt == NULL)
    {
        return 0;
    }
    errno = 0;                         /* reset errno! */
    value = strtof(opt, &end);
    if (errno != 0)
    {
        return 0;
    }
    *value_ptr = value;
    for (conversion = conversions; conversion->name != NULL; ++conversion)
    {
        if (strcmp(end, conversion->name) == 0)
        {
            *value_ptr =
                conversion->convert(*value_ptr, conversion->caller_data);
            return 1;                  /* success: apply conversion */
        }
    }
    errno = EINVAL;
    return 0;                          /* error: no conversion */
}

/*
 * str_convertn() --Parse a string and convert to some units.
 *
 * Parameters:
 * opt  --the string to convert
 * value_ptr --returns the parsed and converted value
 * n_conversion --the number of conversions to try
 * conversions --a list of conversions to try
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int str_convertn(const char *opt, double *value_ptr, size_t n_conversion,
                 Conversion conversions[])
{
    double value;
    char *end;
    ConversionPtr conversion;
    ConversionPtr conversion_end = conversions + n_conversion;

    if (opt == NULL)
    {
        return 0;
    }
    errno = 0;                         /* reset errno! */
    value = strtof(opt, &end);
    if (errno != 0)
    {
        return 0;
    }
    *value_ptr = value;
    for (conversion = conversions; conversion < conversion_end; ++conversion)
    {
        if (strcmp(end, conversion->name) == 0)
        {
            *value_ptr =
                conversion->convert(*value_ptr, conversion->caller_data);
            return 1;                  /* success: apply conversion */
        }
    }
    errno = EINVAL;
    return 0;                          /* error: no conversion */
}

/*
 * scale() --Simple scaling conversion function.
 */
double scale(double val, double *multiplier)
{
    return val * *multiplier;
}

/*
 * linear() --Simple linear (y = mx + c) conversion function.
 */
double linear(double val, LinearConvertArgPtr arg)
{
    return val * arg->m + arg->c;
}
