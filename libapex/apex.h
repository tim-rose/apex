/*
 * APEX.H --Funadamental macros and utilities.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * MACRO_VALUE_STR() --Get the value of a macro as a text string.
 * MACRO_ARG_STR()   --Get the value of a macro argument as a text string.
 * MAX()             --Return the maximum of two numeric values.
 * MIN()             --Return the minimum of two numeric values.
 * ABS()             --Return the absolute of a numeric value.
 * IN_RANGE()        --Test if a value is within a specified range.
 * FEQUAL()          --Test if two float values are suitably close.
 * NEL()             --Return the number of elements in an array.
 * NEW()             --Allocate space for some items of a specified type.
 * SYS_RETRY()       --a lame but portable version of GNU's TEMP_FAILURE_RETRY.
 *
 * See Also:
 * http://unixwiz.net/techtips/gnu-c-attributes.html
 * http://www.ohse.de/uwe/articles/gcc-attributes.html
 */
#ifndef APEX_H
#define APEX_H
/*
 * On cygwin, gcc --std=c99 automatically defines __STRICT_ANSI__ but that
 * removes various prototypes such as strptime, strdup, popen etc etc.
 * Since we want those prototypes, we must undo this, before most system headers
 * are included, hence why this file is included before all others.
 */
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <stdlib.h>
#include <apex/gnuattr.h>

#ifdef __WINNT__
#define SYSLOG <apex/syslog-standalone.h>
#else
#define SYSLOG <syslog.h>
#endif /* __WINNT__ */

#define NULLPTR ((void *) NULL)        /* pointer-sized NULL (for stdarg) */

#ifndef NAME_MAX
#define NAME_MAX 256
#endif /* NAME_MAX */

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif /* LINE_MAX */


/*
 * MACRO_VALUE_STR() --Get the value of a macro as a text string.
 * MACRO_ARG_STR() --Get the value of a macro argument as a text string.
 *
 * Remarks:
 * This macro is useful for creating size-limited `scanf()` formats:
 * ```
 * #define TEXT_SIZE 100
 * char *first_word(const char *str)
 * {
 *     static char text[TEXT_SIZE+1];
 *     static char format[] = "%" MACRO_VALUE_STR(TEXT_SIZE) "s";
 *     if (sscanf(str, format, text) == 1)
 *     {
 *         return text;
 *     }
 *     else
 *     {
 *         return NULL;
 *     }
 * }
 * ```
 * See Also:
 * https://gcc.gnu.org/onlinedocs/gcc-11.2.0/cpp/Stringizing.html
 */
#define MACRO_VALUE_STR(s) MACRO_ARG_STR(s)
#define MACRO_ARG_STR(s) #s
#define STRINGIFY(s) MACRO_VALUE_STR(s)

/*
 * MAX() --Return the maximum of two numeric values.
 * MIN() --Return the minimum of two numeric values.
 * ABS() --Return the absolute of a numeric value.
 *
 * Remarks:
 * Arguments to these macros must be side-effect free.
 */
#define MAX(a_, b_)	((a_) > (b_) ? (a_) : (b_))
#define MIN(a_, b_)	((a_) < (b_) ? (a_) : (b_))
#define ABS(a_)	((a_) >= 0 ? (a_) : -(a_))

/*
 * IN_RANGE() --Test if a value is within a specified range.
 * FEQUAL() --Test if two float values are suitably close.
 *
 * REVISIT: This code doesn't handle 0.0s gracefully.  For a better approach see below.
 *
 * See Also:
 * http://floating-point-gui.de/errors/comparison/
 */
#define IN_RANGE(a_, min_, max_) ((a_)-(min_) < (max_)-(min_))
#define FEQUAL(x_, y_, err_) \
    (ABS((ABS(x_)-ABS(y_)))/(ABS(x_)+ABS(y_)+(err_)) <= (err_))

/*
 * NEL() --Return the number of elements in an array.
 * NEW() --Allocate space for some items of a specified type.
 */
#define NEL(array_)	(sizeof array_ / sizeof array_[0])
#define NEW(type_, nel_)	calloc((nel_), sizeof(type_))

/*
 * SYS_RETRY() --a lame but portable version of GNU's TEMP_FAILURE_RETRY.
 */
#ifdef TEMP_FAILURE_RETRY
#define SYS_RETRY(result_, expression_) \
    result_ = TEMP_FAILURE_RETRY(expression_)
#else
#define SYS_RETRY(result_, expression_) \
    do { result_ = (expression_); } \
    while (result_ == -1 && (errno == EINTR || errno == EAGAIN))
#endif /* TEMP_FAILURE_RETRY */
extern const char *apex_version;
#endif /* APEX_H */
