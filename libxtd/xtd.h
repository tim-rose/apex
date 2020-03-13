/*
 * XTD.H --Funadamental macros and utilities.
 *
 * Contents:
 * MAX()       --Return the maximum of two numeric values.
 * MIN()       --Return the minimum of two numeric values.
 * ABS()       --Return the absolute of a numeric value.
 * IN_RANGE()  --Test if a value is within a specified range.
 * FEQUAL()    --Test if two float values are suitably close.
 * NEL()       --Return the number of elements in an array.
 * NEW()       --Allocate space for some items of a specified type.
 * SYS_RETRY() --a lame but portable version of GNU's TEMP_FAILURE_RETRY.
 *
 * See Also:
 * http://unixwiz.net/techtips/gnu-c-attributes.html
 * http://www.ohse.de/uwe/articles/gcc-attributes.html
 */
#ifndef XTD_H
#define XTD_H
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
#include <gnuattr.h>

#define NULLPTR ((void *) NULL)

#ifndef NAME_MAX
#define NAME_MAX 256
#endif /* NAME_MAX */

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif /* LINE_MAX */

/*
 * MAX() --Return the maximum of two numeric values.
 * MIN() --Return the minimum of two numeric values.
 * ABS() --Return the absolute of a numeric value.
 */
#define MAX(_a_, _b_)	((_a_) > (_b_) ? (_a_) : (_b_))
#define MIN(_a_, _b_)	((_a_) < (_b_) ? (_a_) : (_b_))
#define ABS(_a_)	((_a_) >= 0 ? (_a_) : -(_a_))

/*
 * IN_RANGE() --Test if a value is within a specified range.
 * FEQUAL() --Test if two float values are suitably close.
 *
 * REVISIT: This code doesn't handle 0.0s gracefully.  For a better approach see below.
 *
 * See Also:
 * http://floating-point-gui.de/errors/comparison/
 */
#define IN_RANGE(_a_, _min_, _max_) ((_a_)-(_min_) < (_max_)-(_min_))
#define FEQUAL(_x_, _y_, _err_) \
    (ABS((ABS(_x_)-ABS(_y_)))/(ABS(_x_)+ABS(_y_)+(_err_)) <= (_err_))

/*
 * NEL() --Return the number of elements in an array.
 * NEW() --Allocate space for some items of a specified type.
 */
#define NEL(_array_)	(sizeof _array_ / sizeof _array_[0])
#define NEW(_type_, _nel_)	calloc((_nel_), sizeof(_type_))

/*
 * SYS_RETRY() --a lame but portable version of GNU's TEMP_FAILURE_RETRY.
 */
#ifdef TEMP_FAILURE_RETRY
#define SYS_RETRY(_result_, _expression_) \
    _result_ = TEMP_FAILURE_RETRY(_expression_)
#else
#define SYS_RETRY(_result_, _expression_) \
    do { _result_ = (_expression_); } \
    while (_result_ == -1 && (errno == EINTR || errno == EAGAIN))
#endif /* TEMP_FAILURE_RETRY */
#endif /* XTD_H */
