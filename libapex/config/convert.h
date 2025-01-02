/*
 * CONVERT.H --Conversion routines, and a means of applying them.
 */
#ifndef APEX_CONVERT_H
#define APEX_CONVERT_H

#include <unistd.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef double (*ConvertProc)(double data, void *caller_data);

    typedef struct Conversion
    {
        const char *name;
        ConvertProc convert;
        void *caller_data;
    } Conversion, *ConversionPtr;

    typedef struct LinearConvertArg
    {
        double m, c;
    } LinearConvertArg, *LinearConvertArgPtr;

    extern Conversion convert_length[];
    extern Conversion convert_duration[];
    extern Conversion convert_velocity[];
    extern Conversion convert_temperature[];
    extern Conversion convert_mass[];

    int str_convert(const char *opt, double *value_ptr,
                    Conversion conversions[]);
    int str_convertn(const char *opt, double *value_ptr,
                     size_t n_conversion, Conversion conversions[]);

    double scale(double val, double *multiplier);
    double linear(double val, LinearConvertArgPtr arg);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_CONVERT_H */
