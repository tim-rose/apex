/*
 * TEST.H --Definitions and macros for unit testing.
 *
 * Remarks:
 * These macros extend the basic TAP routines with some facilities
 * inspired from perl's Test::More package.
 */
#ifndef TEST_H
#define TEST_H

#include <xtd.h>                       /* for FEQUAL */
#include <xtd/estring.h>                   /* for strequiv */
#include <xtd/tap.h>

#define ptr_eq(have_, expected_, ...)          \
    (ok(((void *) have_) == (((void *) expected_)), __VA_ARGS__) ? 1 : \
        (diag("%10s: 0x%p", "got", STR_OR_NULL(have_)), \
        diag("%10s: 0x%p", "expected", STR_OR_NULL(expected_)), 0))

#define number_eq(have_, expected_, number_fmt_, ...)          \
    (ok((have_) == (expected_), __VA_ARGS__) ? 1 :      \
        (diag("%10s: " number_fmt_, "got", (have_)), \
         diag("%10s: " number_fmt_, "expected", (expected_)), 0))

#define float_eq(have_, expected_, float_fmt_, _tolerance, ...)             \
    (ok(FEQUAL((have_), (expected_), (_tolerance)) == 0, __VA_ARGS__) ? 1 : \
        (diag("%10s: " float_fmt_, "got", (have_)), \
         diag("%10s: " float_fmt_, "expected", (expected_)), 0))

#define ok_number(have_, op_, expected_, number_fmt_, ...)    \
    (ok((have_) op_ (expected_), __VA_ARGS__) ? 1 :      \
        (diag("%10s: " number_fmt_, "got", (have_)), \
         diag("%10s: a value %s " number_fmt_, "expected", #op_, (expected_)), 0))

#define string_eq(have_, expected_, ...)                          \
    (ok(strequiv((have_), (expected_)) == 0, __VA_ARGS__) ? 1 :   \
        (diag("%10s: \"%s\"", "got", STR_OR_NULL(have_)), \
         diag("%10s: \"%s\"", "expected", STR_OR_NULL(expected_)), 0))

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

#endif                                 /* TEST_H */
