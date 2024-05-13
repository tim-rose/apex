/*
 * GETOPTS.H --a vararg-style getopt() variant.
 *
 */
#ifndef GETOPTS_H
#define GETOPTS_H

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
#endif                                 /* GETOPTS_H */
