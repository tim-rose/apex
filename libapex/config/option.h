/*
 * OPTION.H --Option processing.
 *
 * Remarks:
 * The Option structure specifies names for various values
 * that are of interest to the program.  The options are processed
 * into getopt() spec.s and can be provided on the command-line in
 * the usual way.  It will also search for and load the values from
 * a config file.
 *
 */
#ifndef APEX_OPTION_H
#define APEX_OPTION_H

#include <apex.h>
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    /*
     * OptionProc() --config setting action routine.
     *
     * Parameters:
     * value    --the new value as a string to be parsed
     * data --caller's function-specific data (i.e. "this")
     *
     * Returns: (int)
     * Success: 1; Failure: 0.
     */
    typedef int (*OptionProc)(const char *name, const char *text, void *data);

    typedef struct Option_t
    {
        char opt;                      /* single-char option, if any */
        const char *name;              /* long-opt/configuration name */
        const char *value_name;        /* long-opt/configuration value name */
        const char *value;             /* default value (to be parsed!) */
        const char *doc;               /* documentation for this variable */
        OptionProc proc;               /* convert/validate handler function */
        void *data;                    /* address to receive option value */
        int set;                       /* flag: has this option been set already? */
    } Option, *OptionPtr, *Options;

    int opt_defaults(Option opts[]);
    int opt_getopts(int argc, char *argv[], Option opts[]);
    int opt_getopts_long(int argc, char *argv[], Option opts[]);
    void opt_usage(const char *prologue, Option opts[], const char *epilogue);

    /*
     * OptProc conversion routines...
     */
    int opt_string(const char *name, const char *text, void *data);
    int opt_bool(const char *name, const char *text, void *data);
    int opt_int(const char *name, const char *text, void *data);
    int opt_float(const char *name, const char *text, void *data);
    int opt_double(const char *name, const char *text, void *data);

    int opt_length(const char *name, const char *text, void *data);
    int opt_velocity(const char *name, const char *text, void *data);
    int opt_duration(const char *name, const char *text, void *data);
    int opt_temperature(const char *name, const char *text, void *data);

    int opt_timestamp(const char *name, const char *text, void *data);
    int opt_input_file(const char *UNUSED(name), const char *file_name,
                       void *data);

    int opt_log_level(const char *name, const char *text, void *unused);

    extern char opt_log_inherit[];     /* used by opt_log_level() */

    /*
     * OPTION_LOG --boilerplate command line options for setting log level.
     *
     * Remarks:
     * This macro defines a block of Option initialisations that
     * implement (mostly boolean) command-line options for setting the
     * logging level.  If you include this macro in your Option list,
     * the following command line options apply:
     * * -v, --verbose  (set logging level to "info")
     * * -q, --quiet    (set logging level to "notice")
     * * -_, --debug    (set logging level to "debug")
     * * --log-level=<debug|info|notice|warn...etc>
     *
     * Note that the for C++ compatibility C99's "designated initialisers"
     * are NOT be used (e.g., { .opt = 'q', .name = "quiet", etc. } )
     */
#define OPTION_LOG \
    /* opt, name, value, doc, proc, data... */ \
    { 0, "log-level", "syslog-level", \
      opt_log_inherit, "set the logging level to the specified syslog priority", \
      opt_log_level, NULL, 0 }, \
    { 'q', "quiet", NULL, 0, "set the logging level to \"notice\"",  opt_log_level, NULL, 0 }, \
    { 'v', "verbose", NULL, 0, "set the logging level to \"info\"", opt_log_level, NULL, 0 }, \
    { '_', "debug", NULL, 0, "set the logging level to \"debug\"", opt_log_level, NULL, 0 }
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_OPTION_H */
