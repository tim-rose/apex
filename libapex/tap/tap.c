/*
 * TAP.C --Routines for emitting test results using the TAP protocol.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * CHARS_REMAINING() --Calculate the number of remaining char.s in a buffer
 * tap_init_()       --Initialise the TAP library.
 * tap_exit_()       --End-of-process cleanup for the TAP module.
 * tap_fatal_()      --Report a fatal message, and abort the test run.
 * tap_escape_()     --Escape occurances of a character in a text buffer.
 * tap_format_()     --Format some user-supplied text for a TAP record.
 * tap_result()      --Print a test result in TAP format.
 * plan_skip_all()   --Note that the plan is to skip all tests.
 * diag()            --Print a diagnostic message to stderr.
 * skip()            --Skip some tests, reporting a reason.
 * todo_start()      --Start a block of "TODO" (expected-to-fail) tests.
 * todo_end()        --Close a block of "TODO" tests.
 * exit_status()     --Calculate the exit status for this test suite.
 *
 * Remarks:
 * This is an adaption/re-write of Nik Clayton's original TAP library.
 * The major differences are that I'm avoiding malloc in favour of
 * clamping the output record size to (currently) 4K, and using
 * internal buffering/formatting to construct it.  Avoiding malloc
 * is important for programs that want to simulate memory failure
 * by linking against a custom malloc.
 *
 * TODO: get TAP styles from TAP_COLORS
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <apex/tap.h>

/*
 * CHARS_REMAINING() --Calculate the number of remaining char.s in a buffer
 *
 * Parameters:
 * _base    --the base address of the (assumed char[]) buffer
 * _end     --the current "end" of the string stored in the buffer.
 *
 */
#define CHARS_REMAINING(_base, _end) (size_t)(_base + sizeof(_base) - (_end))

/*
 * TapState --the state variables for a TAP test suite.
 *
 * Remarks:
 * This structure captures the current running state of the TAP
 * test suite.  The values are mostly straightforward, however
 * n_plan's values are a little special:
 * * 0   --no declared plan (i.e. implicitly no plan)
 * * -1  --declared no plan (i.e. explicitly stated that there's no plan)
 * * +ve --the No. of planned tests.
 */
typedef struct TapState
{
    int n_plan;                        /* No. planned tests */
    int n_test;                        /* No. tests attempted so far */
    int n_fail;                        /* No. tests that failed */
    int n_skip;                        /* No. tests skipped */
    int abort;                         /* "test run aborted" flag */
    int todo;                          /* "in TODO block" mode flag */
    int skip;                          /* "skipping all tests" mode flag */
    char todo_text[TAP_LINE_MAX];
} TapState;

static void tap_init_(void);
static void tap_exit_(void);
static void tap_fatal_(int result, const char *message);
static char *tap_escape_(char *text, size_t size,
                         int ch, const char *replacement);
static int tap_format_(char *text, size_t size,
                       const char *fmt, va_list arglist);

static TapState tap;

static const char *pass_style = "";
static const char *fail_style = "";
static const char *todo_style = "";    /* not used yet! */
static const char *plan_style = "";
static const char *end_style = "";
static const char *diag_style = "";

/*
 * tap_init_() --Initialise the TAP library.
 *
 * Remarks:
 * This routine sets stdout to be unbuffered so that it will be
 * co-mingled with stderr correctly.
 */
static void tap_init_(void)
{
    static int tap_initialised = 0;

    if (!tap_initialised)
    {
        atexit(tap_exit_);
        setvbuf(stdout, NULL, _IONBF, 0);   /* no buffering */
        tap_initialised = 1;
    }
    if (isatty(1))
    {                                  /* coloured output to stdout */
        pass_style = "\033[32m";       /* green */
        fail_style = "\033[1;31m";     /* bold red */
        todo_style = "\033[36m";       /* cyan */
        plan_style = "\033[1m";        /* bold */
        end_style = "\033[m";
    }
    if (isatty(2))
    {                                  /* coloured output to stderr */
        diag_style = "\033[3m";        /* dim */
    }
}

/*
 * tap_exit_() --End-of-process cleanup for the TAP module.
 *
 * Remarks:
 * This routine prints a summary of the test run to stderr.
 */
void tap_exit_(void)
{
    if (tap.n_plan == 0 && !tap.skip)
    {
        diag("Looks like your test died before it could output anything.");
        return;
    }

    if (tap.abort)
    {
        diag("Looks like your test died just after %d.", tap.n_test);
        return;
    }

    if (tap.n_plan <= 0)
    {                                  /* no explicit plan? */
        if (!tap.skip)
        {                              /* synthesise plan posthoc */
            printf("%s1..%d%s\n", plan_style, tap.n_test, end_style);
        }
    }
    else
    {
        if (tap.n_plan < tap.n_test)
        {
            diag("Looks like you planned %d tests but ran %d extra.",
                 tap.n_plan, tap.n_test - tap.n_plan);
            return;
        }
        if (tap.n_plan > tap.n_test)
        {
            diag("Looks like you planned %d tests but only ran %d.",
                 tap.n_plan, tap.n_test);
            return;
        }
    }

    if (tap.n_fail)
    {
        diag("Looks like you failed %d tests of %d.", tap.n_fail, tap.n_test);
    }
}

/*
 * tap_fatal_() --Report a fatal message, and abort the test run.
 */
static void tap_fatal_(int result, const char *message)
{
    if (result)
    {
        diag("%s", message);
        tap.abort = 1;
        exit(255);
    }
}

/*
 * tap_escape_() --Escape occurances of a character in a text buffer.
 *
 * Parameters:
 * text --specifies and returns the text subject to character escapement
 * size --the size of text buffer
 * ch   --the character to be replaced
 * replacement  --the string to replace it with
 *
 * Returns: (char *)
 * Success: a pointer to the end of the string in text; Failure: NULL.
 *
 * Remarks:
 * This routine takes a copy of the string, and then copies it back
 * character-by-character, escaping/replacing as it goes.
 */
static char *tap_escape_(char *text, size_t size,
                         int ch, const char *replacement)
{
    char alt_text[TAP_LINE_MAX];
    size_t replace_len = strlen(replacement);
    char *dst = text;

    strcpy(alt_text, text);

    for (char *src = alt_text; *src != '\0'; ++src)
    {
        if (*src == ch)
        {                              /* escape-worthy character */
            if (size > replace_len)
            {
                strcpy(dst, replacement);
                size -= replace_len;
                dst += replace_len;
            }
            else
            {
                break;                 /* no more room for replacement */
            }
        }
        else
        {
            if (size > 1)
            {
                *dst++ = *src;
                size -= 1;
            }
            else
            {
                break;                 /* no more room for copy */
            }
        }
    }
    *dst = '\0';                       /* terminate string */
    return dst;
}

/*
 * tap_format_() --Format some user-supplied text for a TAP record.
 *
 * Parameters:
 * text --specifies a text buffer that returns the formatted text
 * size --specifies the size of the text buffer
 * fmt  --a printf(3) format string
 * arglist  --a pointer to the arguments for the printf format.
 *
 * Returns: (int)
 * The number of characters written to text.
 */
static int tap_format_(char *text, size_t size,
                       const char *fmt, va_list arglist)
{
    char *end;

    vsnprintf(text, size, fmt, arglist);
    end = tap_escape_(text, size, '\n', "\n# ");
    return end - text;
}

/*
 * tap_result() --Print a test result in TAP format.
 *
 * Parameters:
 * ok -- boolean, indicates whether or not the test passed.
 * test_name -- the name of the test, may be NULL
 * test_comment -- a comment to print afterwards, may be NULL
 *
 * Returns: (int)
 * result, clamped to [0,1].
 */
int tap_result(int result, const char *func,
               const char *file, size_t line, const char *fmt, ...)
{
    va_list arglist;
    char text[TAP_LINE_MAX];
    char user_text[TAP_LINE_MAX];
    char *end = text;

    if (fmt != NULL)
    {                                  /* build/test/clean the user text */
        char *endp;

        va_start(arglist, fmt);
        vsnprintf(user_text, sizeof(user_text), fmt, arglist);
        va_end(arglist);

        errno = 0;
        if (strtol(user_text, &endp, 0) == 0 && *endp == '\0')
        {
            diag("    You named your test '%s'.  "
                 "You shouldn't use numbers for your test names.", user_text);
            diag("    Very confusing.");
        }
        tap_escape_(user_text, sizeof(user_text), '#', "\\#");
    }

    end += sprintf(text, "%sok %d", (result ? "" : "not "), ++tap.n_test);

    if (fmt != NULL)
    {
        snprintf(end, CHARS_REMAINING(text, end),
                 " - %s%s", user_text, tap.todo_text);
    }

    printf("%s%s%s\n", (result ? pass_style : fail_style), text, end_style);

    if (!result)
    {
        if (!tap.todo)
        {                              /* TODO tests expected/allowed to fail */
            ++tap.n_fail;
        }
        diag("    Failed %stest %s (in function %s())\n    at %s line %d.",
             tap.todo ? "(TODO) " : "", text, func, file, line);
    }
    return result != 0;                /* result: clamped to bool values */
}

/*
 * plan_skip_all() --Note that the plan is to skip all tests.
 */
void plan_skip_all(const char *fmt, ...)
{
    va_list arglist;
    char text[TAP_LINE_MAX] = "";
    char *end = text;

    tap_init_();
    tap.skip = 1;

    if (fmt != NULL)
    {
        end += snprintf(text, sizeof(text), "%s", " # Skip ");
        va_start(arglist, fmt);
        tap_format_(text, CHARS_REMAINING(text, end), fmt, arglist);
        va_end(arglist);
    }
    printf("1..0%s\n", text);
    exit(0);
}

/*
 * Note the number of tests that will be run.
 */
void plan_tests(int n_test)
{
    tap_init_();
    tap_fatal_(tap.n_test != 0, "You tried to plan twice");
    tap_fatal_(n_test == 0, "You said to run 0 tests");
    tap.n_plan = n_test;
    if (n_test > 0)
    {                                  /* Emit plan record */
        printf("%s1..%d%s\n", plan_style, tap.n_plan, end_style);
    }
}

/*
 * diag() --Print a diagnostic message to stderr.
 */
void diag(const char *fmt, ...)
{
    va_list arglist;
    char text[TAP_LINE_MAX];
    char *end = text;

    end += snprintf(end, CHARS_REMAINING(text, end), "%s", "# ");
    va_start(arglist, fmt);
    end += tap_format_(end, CHARS_REMAINING(text, end), fmt, arglist);
    va_end(arglist);
    fprintf(stdout, "%s%s%s\n", diag_style, text, end_style);
}

/*
 * skip() --Skip some tests, reporting a reason.
 *
 * Parameters:
 * n_test   --the No. of tests to skip
 * fmt, ... --the message, specified as printf-style arg.s
 */
void skip(size_t n_test, const char *fmt, ...)
{
    va_list arglist;
    char text[TAP_LINE_MAX];

    va_start(arglist, fmt);
    tap_format_(text, sizeof(text), fmt, arglist);
    va_end(arglist);

    while (n_test-- > 0)
    {                                  /* pretend to run the tests... */
        printf("%sok %d # skip %s%s\n",
               pass_style, ++tap.n_test, text, end_style);
    }
}

/*
 * todo_start() --Start a block of "TODO" (expected-to-fail) tests.
 */
void todo_start(const char *fmt, ...)
{
    va_list arglist;
    char *end = tap.todo_text;

    end += sprintf(tap.todo_text, "%s", " # TODO ");
    va_start(arglist, fmt);
    tap_format_(end, CHARS_REMAINING(tap.todo_text, end), fmt, arglist);
    va_end(arglist);

    tap.todo = 1;
}

/*
 * todo_end() --Close a block of "TODO" tests.
 */
void todo_end(void)
{
    tap.todo = 0;
    tap.todo_text[0] = '\0';
}

/*
 * exit_status() --Calculate the exit status for this test suite.
 */
int exit_status(void)
{
    if (tap.n_plan <= 0)
    {                                  /* no plan */
        return tap.n_fail;             /* return failures */
    }
    if (tap.n_plan < tap.n_test)
    {                                  /* too many tests */
        return tap.n_test - tap.n_plan; /* return excess */
    }

    /*
     * Otherwise:
     * Return the number of tests that failed + the number of tests
     * that weren't run
     */
    return tap.n_fail + tap.n_plan - tap.n_test;
}
