/*
 * TEST.H --Definitions and macros for unit testing.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Remarks:
 * These macros extend the basic TAP routines with some facilities
 * inspired from perl's Test::More package.  In particular, when the
 * test fails, they print the actual and expected values as TAP
 * diagnostic messages.
 *
 * Contents:
 * ok_ptr()    --Compare two pointer/address values.
 * ptr_eq()    --Compare two pointer/address values for equality.
 * ok_number() --Compare two numbers.
 * number_eq() --Compare two numbers for equality.
 * float_eq()  --Compare two floating point values for equality.
 * string_eq() --Compare two strings for equality.
 *
 */
#ifndef APEX_TEST_H
#define APEX_TEST_H

#include <apex.h>                       /* for FEQUAL */
#include <apex/estring.h>               /* for strequiv */
#include <apex/tap.h>

/*
 * ok_ptr() --Compare two pointer/address values.
 *
 * Parameters:
 * have --the value to be tested/compared
 * op	--the relationship/operator (e.g. !=, <, >= etc.)
 * expected --the expected value to be compared with `have`
 * ... --printf format and arg.s
 *
 * Remarks:
 * This macro uses CPP stringization to print a diagnostic message that
 * includes the comparison operator.
 * E.g.
 * ok_ptr(0xffffffff, <, 0xff000000)
 * produces...
 *        got: 0xffffffff
 *   expected: a value < 0xff000000
 */
#define ok_ptr(have_, op_, expected_, ...)          \
    (ok(((void *) have_) op_ ((void *) expected_), __VA_ARGS__) ? 1 :      \
        (diag("%10s: 0x%p", "got", (const char *) have_), \
        diag("%10s: a value %s 0x%p", "expected", #op_, (const char *) expected_), 0))

/*
 * ptr_eq() --Compare two pointer/address values for equality.
 *
 * Parameters:
 * have --the value to be tested/compared
 * expected --the expected value to be compared with `have`
 * ... --printf format and arg.s
 *
 * Remarks:
 * E.g.
 * ok_ptr(0xffffffff, 0xff000000)
 * produces...
 *        got: 0xffffffff
 *   expected: 0xff000000
 */
#define ptr_eq(have_, expected_, ...)  \
    (ok(((void *) have_) == ((void *) expected_), __VA_ARGS__) ? 1 :      \
        (diag("%10s: 0x%p", "got", (const char *) have_), \
        diag("%10s:  0x%p", "expected", (const char *) expected_), 0))

/*
 * ok_number() --Compare two numbers.
 *
 * Parameters:
 * have --the number to be tested/compared
 * op	--the relationship/operator (e.g. !=, <, >= etc.)
 * expected --the expected value to be compared with `have`
 * number_fmt --a printf() format specifier for printing the number.
 * ... --printf format and arg.s
 *
 * Remarks:
 * This macro will work with any numeric type, but you MUST provide
 * the printf() format to handle it. Do not use this to compare float
 * values for equality; use float_eq() for that.
 */
#define ok_number(have_, op_, expected_, number_fmt_, ...)    \
    (ok((have_) op_ (expected_), __VA_ARGS__) ? 1 :      \
        (diag("%10s: " number_fmt_, "got", (have_)), \
         diag("%10s: a value %s " number_fmt_, "expected", #op_, (expected_)), 0))

/*
 * number_eq() --Compare two numbers for equality.
 *
 * Parameters:
 * have --the number to be tested/compared
 * expected --the expected value to be compared with `have`
 * number_fmt --a printf() format specifier for printing the number.
 * ... --printf format and arg.s
 */
#define number_eq(have_, expected_, number_fmt_, ...) \
    (ok((have_) == (expected_), __VA_ARGS__) ? 1 :      \
        (diag("%10s: " number_fmt_, "got", (have_)), \
         diag("%10s: " number_fmt_, "expected", (expected_)), 0))

/*
 * float_eq() --Compare two floating point values for equality.
 *
 * Parameters:
 * have --the number to be tested/compared
 * expected --the expected value to be compared with `have`
 * number_fmt --a printf() format specifier for printing the number.
 * ... --printf format and arg.s
 *
 * Remarks:
 * The comparison for equality is made using the FEQUAL() macro
 * which provides a numerically stable approximate result.
 */
#define float_eq(have_, expected_, float_fmt_, _tolerance, ...)             \
    (ok(FEQUAL((have_), (expected_), (_tolerance)) == 0, __VA_ARGS__) ? 1 : \
        (diag("%10s: " float_fmt_, "got", (have_)), \
         diag("%10s: " float_fmt_, "expected", (expected_)), 0))

/*
 * string_eq() --Compare two strings for equality.
 *
 * Parameters:
 * have --the number to be tested/compared
 * expected --the expected value to be compared with `have`
 * ... --printf format and arg.s
 */
#define string_eq(have_, expected_, ...)                          \
    (ok(strequiv((have_), (expected_)) == 0, __VA_ARGS__) ? 1 :   \
        (diag("%10s: \"%s\"", "got", STR_OR_NULL(have_)), \
         diag("%10s: \"%s\"", "expected", STR_OR_NULL(expected_)), 0))

/*
 * Not so sure about these...
 */
#define object_eq(have_, expected_, _compare, _sprint, ...)        \
    (ok(_compare((have_), (expected_)) == 0, __VA_ARGS__) ? 1 : \
        (diag("%10s: %s", "got", _sprint(have_)), \
         diag("%10s: %s", "expected", _sprint(expected_)), 0))

#define str_field_eq(struct_, field_, expected_, ...)        \
    string_eq(struct_.field_, expected_, \
        #struct_ "." #field_ ": " __VA_ARGS__)

#define number_field_eq(struct_, field_, expected_, number_fmt_, ...)  \
    number_eq(struct_.field_, expected_, number_fmt_, \
        #struct_ "." #field_ ": "__VA_ARGS__)

#define ok_number_field(struct_, field_, cmp_, expected_, number_fmt_, ...)    \
    ok_number(struct_.field_, cmp_, expected_, number_fmt_,                    \
        #struct_ "." #field_ ": "__VA_ARGS__)

#endif /* APEX_TEST_H */
