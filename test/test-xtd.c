/*
 * XTD.C --Unit tests for the "xtd" macros.
 *
 * Contents:
 * test_min()    --Test the behaviour of the MIN macro.
 * test_max()    --Test the behaviour of the MAX macro.
 * test_abs()    --Test the behaviour of the ABS macro.
 * test_fequal() --Test the behaviour of the FEQUAL macro.
 *
 */
#include <string.h>

#include <tap.h>
#include <xtd.h>
#include <getopts.h>

typedef struct Opt
{
    const char *a, *b;
    int c, d;
} Opt;

static void test_min(void);
static void test_max(void);
static void test_abs(void);
static void test_fequal(void);

int main(void)
{
    plan_tests(20);

    test_min();
    test_max();
    test_abs();
    test_fequal();

    return exit_status();
}

/*
 * test_min() --Test the behaviour of the MIN macro.
 *
 * Remarks:
 * Note that we're doing equality tests with floats here; usually
 * that's not a good idea, but the way these macros work (i.e. direct
 * assignment) it should not be a problem.
 */
static void test_min(void)
{
    ok(MIN(1, 10) == 1, "min: sorted integer");
    ok(MIN(10, 1) == 1, "min: unsorted integer");

    ok(MIN(1.0, 10.0) == 1.0, "min: sorted float");
    ok(MIN(10.0, 1.0) == 1.0, "min: unsorted float");

    ok(MIN(1, 10.0) == 1.0, "min: sorted integer/float");
    ok(MIN(10, 1.0) == 1.0, "min: unsorted integer/float");
}

/*
 * test_max() --Test the behaviour of the MAX macro.
 */
static void test_max(void)
{
    ok(MAX(1, 10) == 10, "max: sorted integer");
    ok(MAX(10, 1) == 10, "max: unsorted integer");

    ok(MAX(1.0, 10.0) == 10.0, "max: sorted float");
    ok(MAX(10.0, 1.0) == 10.0, "max: unsorted float");

    ok(MAX(1, 10.0) == 10.0, "max: sorted integer/float");
    ok(MAX(10, 1.0) == 10.0, "max: unsorted integer/float");
}

/*
 * test_abs() --Test the behaviour of the ABS macro.
 */
static void test_abs(void)
{
    ok(ABS(1) == 1, "abs: +ve integer");
    ok(ABS(-1) == 1, "abs: -ve integer");

    ok(ABS(1.0) == 1.0, "abs: +ve float");
    ok(ABS(-1.0) == 1.0, "abs: -ve float");
}

/*
 * test_fequal() --Test the behaviour of the FEQUAL macro.
 */
static void test_fequal(void)
{
    ok(FEQUAL(1.0, 1.0, 0.001), "fequal: equal +ve values");
    ok(!FEQUAL(1.0, 2.0, 0.001), "fequal: unequal +ve values");

    ok(FEQUAL(-1.0, -1.0, 0.001), "fequal: equal -ve values");
    ok(!FEQUAL(-1.0, -2.0, 0.001), "fequal: unequal -ve values");
}
