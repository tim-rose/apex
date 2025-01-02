/*
 * GNUATTR.H --Controlled redefinition of GNUC's function/paramater attributes.
 *
 * Contents:
 * UNUSED()             --Annotate a parameter as unused, make it unusable.
 * PRINTF_ATTRIBUTE()   --Annotate a function so GNU C can check printf formats.
 * STRFTIME_ATTRIBUTE() --Annotate a function so GNU C can check strftime formats.
 * NONNULL()            --Annotate that a function cannot tolerate NULL ptrs.
 * NORETURN()           --Annotate a function as never returning.
 * PURE()               --Annotate a function as side-effect free and stateless.
 * WARN_UNUSED()        --Annotate that a function's return value must be used.
 * MALLOC()             --Annotate a function as new heap-allocated data.
 * CONST()              --Annotate that a function depends only on its (non-ptr) params.
 * DEPRECATED()         --Annotate a function/type as deprecated (warn on use).
 * USED()               --Annotate a function variable as used (don't optimise it away).
 *
 * See Also:
 * http://unixwiz.net/techtips/gnu-c-attributes.html
 * http://www.ohse.de/uwe/articles/gcc-attributes.html
 *
 */
#ifndef APEX_GNUATTR_H
#define APEX_GNUATTR_H

/*
 * Setup GNU-ish version information
 *
 */
#ifdef __GNUC__
#define GNUC__ __GNUC__                /* get GNUC's current values... */
#define GNUC_MINOR__ __GNUC_MINOR__
#define GNUC_PATCHLEVEL__ __GNUC_PATCHLEVEL__
#else
#define __attribute__ (x)              /* disable GNUC's extended attributes */
#define GNUC__ 0                       /* define GNUC with a sentinel value */
#define GNUC_MINOR__ 0
#define GNUC_PATCHLEVEL__ 0
#endif /* __GNUC__ */

#ifndef VERSION_NUM
#define VERSION_NUM(_major, _minor, _patch) \
    ((((_major)*100) + _minor)*100 + _patch)
#endif /* VERSION */
#define GCC_VERSION VERSION_NUM(GNUC__, GNUC_MINOR__, GNUC_PATCHLEVEL__)

/*
 * UNUSED() --Annotate a parameter as unused, make it unusable.
 *
 * See Also: http://sourcefrog.net/weblog/software/languages/C/unused.html
 */
#ifndef UNUSED
#if GCC_VERSION >= VERSION_NUM(3, 4, 0)
#define UNUSED(x) /*@unused@*/ UNUSED_ ## x __attribute__((unused))
#else
#define UNUSED(x) /*@unused@*/ UNUSED_ ## x
#endif /* __GNUC__ versions... */
#endif /* UNUSED */

/*
 * PRINTF_ATTRIBUTE() --Annotate a function so GNU C can check printf formats.
 */
#ifndef PRINTF_ATTRIBUTE
#define PRINTF_ATTRIBUTE(arg1, arg2) \
         __attribute__ ((format (__printf__, arg1, arg2)))
#endif /* PRINTF_ATTRIBUTE */

/*
 * STRFTIME_ATTRIBUTE() --Annotate a function so GNU C can check strftime formats.
 */
#ifndef STRFTIME_ATTRIBUTE
#define STRFTIME_ATTRIBUTE(arg1) \
         __attribute__ ((format (__strftime__, arg1, 0)))
#endif /* STRFTIME_ATTRIBUTE */

/*
 * NONNULL() --Annotate that a function cannot tolerate NULL ptrs.
 */
#ifndef NONNULL
#define NONNULL(...) __attribute__((nonnull(##__VA_ARGS__)))
#endif /* NONNULL */

/*
 * NORETURN() --Annotate a function as never returning.
 */
#ifndef NORETURN
#define NORETURN __attribute__((noreturn))
#endif /* NORETURN */

/*
 * PURE() --Annotate a function as side-effect free and stateless.
 *
 * Remarks:
 * Pure functions can be optimised in common sub-expression and loop
 * situations, because they depend only on their parameters and global
 * state (which they don't modify).
 */
#ifndef PURE
#define PURE __attribute__((pure))
#endif /* PURE */

/*
 * WARN_UNUSED() --Annotate that a function's return value must be used.
 *
 * Remarks:
 * Functions that return malloc/realloc data can't be ignored, e.g..
 */
#ifndef WARN_UNUSED
#if GCC_VERSION >= VERSION_NUM(3, 4, 0)
#define WARN_UNUSED __attribute__((warn_unused_result))
#else
#define WARN_UNUSED
#endif /* __GNUC__ versions... */
#endif /* WARN_UNUSED */

/*
 * MALLOC() --Annotate a function as new heap-allocated data.
 *
 */
#ifndef MALLOC
#define MALLOC __attribute__((malloc))
#endif /* MALLOC */

/*
 * CONST() --Annotate that a function depends only on its (non-ptr) params.
 *
 * Remarks:
 * Const functions are memoisable.  Const functions can't call non-const
 * functions.  Most "mathematical" functions (e.g. sin(), sqrt()) are like
 * this.
 */
#ifndef CONST
#define CONST __attribute__((const))
#endif /* CONST */

/*
 * DEPRECATED() --Annotate a function/type as deprecated (warn on use).
 */
#ifndef DEPRECATED
#define DEPRECATED __attribute__((deprecated))
#endif /* DEPRECATED */

/*
 * USED() --Annotate a function variable as used (don't optimise it away).
 */
#ifndef USED
#define USED __attribute__((used))
#endif /* USED */
#endif /* APEX_GNUATTR_H */
