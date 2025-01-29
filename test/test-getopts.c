/*
 * GETOPTS.C --Unit tests for the "getopts" functions.
 *
 * Contents:
 * test_empty_args()    --Test ability to parse/process an empty argv[].
 * test_all_options()   --Test ability to parse/process all options.
 * test_partial_options() --Test ability to parse/process some options only.
 * test_str_inet4()     --Test the behaviour of str_inet4_address().
 * test_str_int()       --Test the behaviour of str_int().
 * test_str_int16()     --Test the behaviour of str_int16().
 * test_str_int16()     --Test the behaviour of str_int16().
 * sprint_int_array()   --Print an array of integers.
 * test_str_list()      --Test integer list parsing.
 * test_str_list()      --Test integer list parsing.
 *
 */
#include <string.h>

#include <apex/test.h>
#include <apex.h>
#include <apex/estring.h>
#include <apex/strparse.h>
#include <apex/getopts.h>
#include <apex/compare.h>

typedef struct Opt
{
    const char *a, *b;
    int c, d;
} Opt;

static Opt null_opt = {.a = 0 };


static void test_empty_args(void);
static void test_all_options(void);
static void test_partial_options(void);
static void test_str_inet4(void);
static void test_str_int(void);
static void test_str_int16(void);
static void test_str_double(void);
static void test_str_list(void);
static void test_str_str_list(void);

int main(void)
{
    plan_tests(53);

    test_empty_args();
    test_all_options();
    test_partial_options();
    test_str_inet4();
    test_str_int();
    test_str_int16();
    test_str_double();
    test_str_list();
    test_str_str_list();

    return exit_status();
}

/*
 * test_empty_args() --Test ability to parse/process an empty argv[].
 */
static void test_empty_args(void)
{
    Opt opt = null_opt;
    const char *args[] = { "test" };

    optind = 1;
    ok(getopts(NEL(args), args, "a:b:cd",
               &opt.a, &opt.b, &opt.c, &opt.d), "empty args");
}

/*
 * test_all_options() --Test ability to parse/process all options.
 */
static void test_all_options(void)
{
    Opt opt = null_opt;
    const char *args[] = { "test", "-a", "x", "-b", "y", "-c", "-d" };

    optind = 1;
    ok(getopts(NEL(args), args, "a:b:cd",
               &opt.a, &opt.b, &opt.c, &opt.d), "can parse all options");
    ok(strcmp(opt.a, "x") == 0
       && strcmp(opt.b, "y") == 0
       && opt.c && opt.d, "can process all values (all options)");
    /* TODO: test for failure on unspecified options */
}

/*
 * test_partial_options() --Test ability to parse/process some options only.
 *
 * Remarks:
 * Note that we have to set optind to reset getopt()'s behaviour
 * between tests.
 */
static void test_partial_options(void)
{

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
           && opt.c && !opt.d, "can parse subset of options");
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
           && opt.c && opt.d, "subset options not parsed remain unaltered");
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
       "str_inet(): cannot parse domain names");
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
    /* TODO: test min/max values */
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
    /* TODO: test min/max values */
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

#ifdef SPRINT_INT_ARRAY
/*
 * sprint_int_array() --Print an array of integers.
 *
 * Remarks:
 * Uh, I don't have a use for this yet, maybe one day.
 * Part of the design is to allow for two calls of the
 * function to not trash each other's output.
 */
static char *sprint_int_array(const int array[], size_t n_el)
{
    static char text_buffer[2][100];
    static int slot;
    char *base = text_buffer[slot];
    char *text = base;
    const char *delimiter = ", ";

    slot = (slot+1) % 2;
    strcpy(text, "[");
    text += 1;

    for (int i=0; i<n_el; ++i)
    {
        if (i != 0)
        {
            text += sprintf(text, "%s", delimiter);
        }
        text += sprintf(text, "%d", array[i]);
    }
    strcpy(text, "]");
    return base;
}
#endif /* SPRINT_INT_ARRAY */

/*
 * test_str_list() --Test integer list parsing.
 */
static void test_str_list(void)
{
    int value[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int opt_value[NEL(value)];

    ok(str_int_list("1", NEL(opt_value), opt_value),
       "str_list: simple value parse");
    number_eq(array_cmp(value, opt_value, 1, sizeof(value[0]), int_cmp), 0, "%d",
              "str_list: simple value results");

    ok(str_int_list("1,2", NEL(opt_value), opt_value),
       "str_list: two values parse");
    ok(array_cmp(value, opt_value, 2, sizeof(value[0]), int_cmp) == 0,
       "str_list: two value results");

    ok(str_int_list("1,2,3,4", NEL(opt_value), opt_value),
       "str_list: four values parse");
    ok(array_cmp(value, opt_value, 4, sizeof(value[0]), int_cmp) == 0,
       "str_list: four value results");

    ok(str_int_list("1-4", NEL(opt_value), opt_value),
       "str_list: simple range");
    ok(array_cmp(value, opt_value, 4, sizeof(value[0]), int_cmp) == 0,
       "str_list: simple range results");

    ok(str_int_list("1-4,5-10", NEL(opt_value), opt_value),
       "str_list: list of ranges");
    ok(array_cmp(value, opt_value, 10, sizeof(value[0]), int_cmp) == 0,
       "str_list: list of range results");

    ok(!str_int_list("bogus", NEL(value), value), "str_list: bogus list");
    ok(!str_int_list("10-1", NEL(value), value), "str_list: bad range");
    ok(!str_int_list("1,2,3,4,5,6,7,8,9,10,11,12", NEL(value), value),
       "str_list: list overflow");
    ok(!str_int_list("1-100", NEL(value), value), "str_list: range overflow");
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
    ok(array_cmp(value, opt_value, 1, sizeof(value[0]), strp_cmp) == 0,
       "%s: simple value results", func);

    ok(str_str_list(strcpy(text, "a,b"), NEL(opt_value), opt_value) == 2,
       "%s: ',' delimiter", func);
    ok(array_cmp(value, opt_value, 2, sizeof(value[0]), strp_cmp) == 0,
       "%s: ',' delimiter results", func);

    ok(str_str_list(strcpy(text, "a b"), NEL(opt_value), opt_value) == 2,
       "%s: ' ' delimiter", func);
    ok(array_cmp(value, opt_value, 2, sizeof(value[0]), strp_cmp) == 0,
       "%s: ' ' delimiter results", func);

    ok(str_str_list(strcpy(text, "a b"), NEL(opt_value), opt_value) == 2,
       "%s: ','+' ' delimiter", func);
    ok(array_cmp(value, opt_value, 2, sizeof(value[0]), strp_cmp) == 0,
       "%s: ','+' ' delimiter results", func);

    ok(str_str_list(strcpy(text, " a b"), NEL(opt_value), opt_value) == 2,
       "%s: leading space", func);
    ok(array_cmp(value, opt_value, 2, sizeof(value[0]), strp_cmp) == 0,
       "%s: leading space results", func);

    ok(str_str_list(strcpy(text, "a b "), NEL(opt_value), opt_value) == 2,
       "%s: trailing space", func);
    ok(array_cmp(value, opt_value, 2, sizeof(value[0]), strp_cmp) == 0,
       "%s: trailing space results", func);
    ok(str_str_list(strcpy(text, "a,b"), 1, opt_value) == 0,
       "%s: overflow", func);
}
