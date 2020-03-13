/*
 * TEST.H --Definitions and macros for unit testing.
 *
 * Remarks:
 * These macros extend the basic TAP routines with some facilities
 * inspired from perl's Test::More package.
 */
#ifndef TEST_H
#define TEST_H

#include <tap.h>
#include <xtd.h>                       /* for FEQUAL */
#include <estring.h>                   /* for strequiv */

#define int_is(_have, _expected, _format, ...)          \
    (ok((_have)-(_expected) == 0, __VA_ARGS__) ? 1 :      \
        (diag("%10s: " _format, "got", (_have)), \
         diag("%10s: " _format, "expected", (_expected)), 0))

#define float_is(_have, _expected, _format, _tolerance, ...)             \
    (ok(FEQUAL((_have), (_expected), (_tolerance)) == 0, __VA_ARGS__) ? 1 : \
        (diag("%10s: " _format, "got", (_have)), \
         diag("%10s: " _format, "expected", (_expected)), 0))

#define string_is(_have, _expected, ...) \
    (ok(strequiv((_have), (_expected)) == 0, __VA_ARGS__) ? 1 :   \
        (diag("%10s: \"%s\"", "got", STR_OR_NULL(_have)), \
         diag("%10s: \"%s\"", "expected", STR_OR_NULL(_expected)), 0))

#define object_is(_have, _expected, _compare, _sprint, ...)        \
    (ok(_compare((_have), (_expected)) == 0, __VA_ARGS__) ? 1 : \
        (diag("%10s: %s", "got", _sprint(_have)), \
         diag("%10s: %s", "expected", _sprint(_expected)), 0))

#define str_field_is(_struct, _field, _value, _msg, ...)        \
    string_is(_struct._field, _value, _msg ": %s.%s value",\
              __VA_ARGS__, #_struct, #_field)

#define int_field_is(_struct, _field, _value, _msg, ...)        \
    int_is(_struct._field, _value, "%d", _msg ": %s.%s value", \
           __VA_ARGS__, #_struct, #_field)

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* TEST_H */
