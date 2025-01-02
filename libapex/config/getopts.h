/*
 * GETOPTS.H --a vararg-style getopt() variant.
 *
 */
#ifndef APEX_GETOPTS_H
#define APEX_GETOPTS_H

#include <unistd.h>
#include <stdint.h>
#include <getopt.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    int getopts(int argc, const char *argv[], const char *opts, ...);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_GETOPTS_H */
