/*
 * CONFIG.H --Config and option processing.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Remarks:
 * The Option structure specifies names for various values
 * that are of interest to the program.  The options are processed
 * into getopt() spec.s and can be provided on the command-line in
 * the usual way.  It will also search for and load the values from
 * a config file.
 */
#ifndef APEX_CONFIG_H
#define APEX_CONFIG_H

#include <apex.h>
#include <apex/option.h>
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */

    int config_load(int argc, char *argv[], const char *config_file,
                    Option opts[]);
    int config_load_ini(const char *file, const char *section, Option opts[]);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_CONFIG_H */
