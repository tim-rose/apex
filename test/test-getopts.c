/*
 * GETOPTS.C --Unit tests for the "getopts" functions.
 *
 * Contents:
 * test_getopts()    --Test the behaviour of getopts().
 * test_str_inet4()  --Test the behaviour of str_inet4_address().
 * test_str_int()    --Test the behaviour of str_int().
 * test_str_int16()  --Test the behaviour of str_int16().
 * test_str_int16()  --Test the behaviour of str_int16().
 * array_equal()     --Test if two integer arrays are equal.
 * test_str_list()   --Test integer list parsing.
 * str_array_equal() --Test if two integer arrays are equal.
 * test_str_list()   --Test integer list parsing.
 *
 */
#include <string.h>

#include <test.h>
#include <xtd.h>
#include <estring.h>
#include <strparse.h>
#include <getopts.h>

typedef struct Opt
{
    const char *a, *b;
    int c, d;
} Opt;

static Opt null_opt = {.a = 0 };


static void test_getopts(void);
static void test_str_inet4(void);
static void test_str_int(void);
static void test_str_int16(void);
static void test_str_double(void);
static int array_equal(size_t n, int a[], int b[]);
static void test_str_list(void);
static int str_array_equal(size_t n, char *a[], char *b[]);
static void test_str_str_list(void);

int main(void)
{
    plan_tests(53);

    test_getopts();
    test_str_inet4();
    test_str_int();
    test_str_int16();
    test_str_double();
    test_str_list();
    test_str_str_list();

    return exit_status();
}

/*
 * test_getopts() --Test the behaviour of getopts().
 *
 * Remarks:
 * Note that we have to set optind to reset getopt()'s behaviour
 * between tests.
 */
static void test_getopts(void)
{
    do
    {
        Opt opt = null_opt;
        const char *args[] = { "test" };

        ok(getopts(NEL(args), args, "a:b:cd",
                   &opt.a, &opt.b, &opt.c, &opt.d), "getopts(): empty args");
    }
    while (0);

    optind = 1;
    do
    {
        Opt opt = null_opt;
        const char *args[] = { "test", "-a", "x", "-b", "y", "-c", "-d" };

        ok(getopts(NEL(args), args, "a:b:cd",
                   &opt.a, &opt.b, &opt.c, &opt.d), "getopts(): all options");
        ok(strcmp(opt.a, "x") == 0
           && strcmp(opt.b, "y") == 0
           && opt.c && opt.d, "getopts(): values (all options)");
    }
    while (0);

    optind = 1;
    do
    {
        Opt opt = null_opt;
        const char *args[] = { "test", "-a", "x", "-c" };

        ok(getopts(NEL(args), args, "a:b:cd",
                   &opt.a, &opt.b, &opt.c, &opt.d),
           "getopts(): partial options");
        ok(strcmp(opt.a, "x") == 0
           && opt.b == NULL
           && opt.c && !opt.d, "getopts(): values (partial options)");
    }
    while (0);
    optind = 1;
    do
    {
        Opt opt = {.a = "foo",.b = "bar",.c = 1,.d = 1 };
        const char *args[] = { "test", "-a", "x", "-c" };

        getopts(NEL(args), args, "a:b:cd", &opt.a, &opt.b, &opt.c, &opt.d);
        ok(strcmp(opt.a, "x") == 0
           && strcmp(opt.b, "bar") == 0
           && opt.c && opt.d, "getopts(): default values (partial options)");
    }
    while (0);
}

/*
 * test_str_inet4() --Test the behaviour of str_inet4_address().
 */
static void test_str_inet4(void)
{
    uint32_t addr, mask;

    ok(!str_inet4_address(NULL, &addr, NULL), "str_inet(): NULL addr");
    ok(str_inet4_address("1.2.3.4", &addr, NULL)
       && addr == 0x01020304, "str_inet(): simple IP address");
    addr = 0;
    ok(str_inet4_address("1.2.3.4/16", &addr, &mask)
       && addr == 0x01020304 && mask == 0xffff0000,
       "str_inet(): IP address+mask-bits");
    ok(str_inet4_address("1.2.3.4/255.255.0.0", &addr, &mask)
       && addr == 0x01020304 && mask == 0xffff0000,
       "str_inet(): IP address+mask-decimal");
    addr = 0;
    ok(str_inet4_address("1.2.3.4/16", &addr, NULL)
       && addr == 0x01020304, "str_inet(): IP address, mask ignored");
    ok(!str_inet4_address("localhost", &addr, NULL),
       "str_inet(): name resolution not supported");
}

/*
 * test_str_int() --Test the behaviour of str_int().
 */
static void test_str_int(void)
{
    int32_t value;

    ok(!str_int(NULL, &value), "str_int(): NULL value");
    ok(str_int("123", &value) && value == 123, "str_int(): OK value");
    ok(str_int("-123", &value) && value == -123, "str_int(): negative value");
    ok(str_int("0xff", &value) && value == 0xff, "str_int(): hex value");
    ok(!str_int("foobar", &value), "str_int(): bad value");
}

/*
 * test_str_int16() --Test the behaviour of str_int16().
 */
static void test_str_int16(void)
{
    int16_t hvalue;

    ok(!str_int16(NULL, &hvalue), "str_int16(): NULL value");
    ok(str_int16("123", &hvalue) && hvalue == 123, "str_int16(): OK value");
    ok(str_int16("-123", &hvalue)
       && hvalue == -123, "str_int16(): negative value");
    ok(!str_int16("foobar", &hvalue), "str_int16(): bad value");
}

/*
 * test_str_int16() --Test the behaviour of str_int16().
 */
static void test_str_double(void)
{
    double dvalue;

    ok(!str_double(NULL, &dvalue), "str_double(): NULL value");
    ok(str_double("123", &dvalue)
       && FEQUAL(dvalue, 123, 0.0001), "str_double(): OK value");
    ok(str_double("-123", &dvalue)
       && FEQUAL(dvalue, -123, 0.0001), "str_double(): negative value");
    ok(str_double("123.0e12", &dvalue)
       && FEQUAL(dvalue, 123.0e12, 0.0001), "str_double(): exponent value");
    ok(!str_double("foobar", &dvalue), "str_double(): bad value");
}

/*
 * array_equal() --Test if two integer arrays are equal.
 *
 * Parameters:
 * n    --the number of elements in each array
 * a, b --the two arrays to compare
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
static int array_equal(size_t n, int a[], int b[])
{
    for (size_t i = 0; i < n; ++i)
    {
        if (a[i] != b[i])
        {
            return 0;
        }
    }
    return 1;
}

/*
 * test_str_list() --Test integer list parsing.
 */
static void test_str_list(void)
{
    int value[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int opt_value[NEL(value)];

    ok(str_int_list("1", NEL(opt_value), opt_value),
       "str_list: simple value parse");
    ok(array_equal(1, value, opt_value), "str_list: simple value results");

    ok(str_int_list("1,2", NEL(opt_value), opt_value),
       "str_list: two values parse");
    ok(array_equal(2, value, opt_value), "str_list: two value results");

    ok(str_int_list("1,2,3,4", NEL(opt_value), opt_value),
       "str_list: four values parse");
    ok(array_equal(4, value, opt_value), "str_list: four value results");

    ok(str_int_list("1-4", NEL(opt_value), opt_value),
       "str_list: simple range");
    ok(array_equal(4, value, opt_value), "str_list: simple range results");

    ok(str_int_list("1-4,5-10", NEL(opt_value), opt_value),
       "str_list: list of ranges");
    ok(array_equal(10, value, opt_value), "str_list: list of range results");

    ok(!str_int_list("bogus", NEL(value), value), "str_list: bogus list");
    ok(!str_int_list("10-1", NEL(value), value), "str_list: bad range");
    ok(!str_int_list("1,2,3,4,5,6,7,8,9,10,11,12", NEL(value), value),
       "str_list: list overflow");
    ok(!str_int_list("1-100", NEL(value), value), "str_list: range overflow");
}

/*
 * str_array_equal() --Test if two integer arrays are equal.
 *
 * Parameters:
 * n    --the number of elements in each array
 * a, b --the two arrays to compare
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
static int str_array_equal(size_t n, char *a[], char *b[])
{
    for (size_t i = 0; i < n; ++i)
    {
        if (strequiv(a[i], b[i]) != 0)
        {
            return 0;
        }
    }
    return 1;
}

/*
 * test_str_list() --Test integer list parsing.
 */
static void test_str_str_list(void)
{
    char *value[] = { (char *) "a", (char *) "b" };
    char *opt_value[NEL(value)];
    const char *func = "str_str_list";
    char text[NAME_MAX];

    ok(str_str_list(strcpy(text, "a"), NEL(opt_value), opt_value) == 1,
       "%s: simple value parse", func);
    ok(str_array_equal(1, value, opt_value), "%s: simple value results",
       func);

    ok(str_str_list(strcpy(text, "a,b"), NEL(opt_value), opt_value) == 2,
       "%s: ',' delimiter", func);
    ok(str_array_equal(2, value, opt_value), "%s: ',' delimiter results",
       func);

    ok(str_str_list(strcpy(text, "a b"), NEL(opt_value), opt_value) == 2,
       "%s: ' ' delimiter", func);
    ok(str_array_equal(2, value, opt_value), "%s: ' ' delimiter results",
       func);

    ok(str_str_list(strcpy(text, "a b"), NEL(opt_value), opt_value) == 2,
       "%s: ','+' ' delimiter", func);
    ok(str_array_equal(2, value, opt_value), "%s: ','+' ' delimiter results",
       func);

    ok(str_str_list(strcpy(text, " a b"), NEL(opt_value), opt_value) == 2,
       "%s: leading space", func);
    ok(str_array_equal(2, value, opt_value), "%s: leading space results",
       func);

    ok(str_str_list(strcpy(text, "a b "), NEL(opt_value), opt_value) == 2,
       "%s: trailing space", func);
    ok(str_array_equal(2, value, opt_value), "%s: trailing space results",
       func);
    ok(str_str_list(strcpy(text, "a,b"), 1, opt_value) == 0,
       "%s: overflow", func);
}
