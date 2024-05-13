/*
 * ESTRING.C --Unit tests for the estring module.
 *
 * Contents:
 * test_strempty()   --strempty tests.
 * test_strtrunc()   --strtrunc tests.
 * test_strcpy()     --strcpy tests.
 * test_vstrcat()    --vstrcat tests.
 * test_strtolower() --strtolower tests.
 * test_strtoupper() --strtoupper tests.
 * test_strsub()     --strsub tests.
 * test_strtr()      --strtr tests.
 * test_vstrmatch()  --vstrmatch tests.
 * main()            --Tests entrypoint.
 */
#include <apex/test.h>
#include <apex/estring.h>

#define DIGITS "0123456789"

/*
 * test_strempty() --strempty tests.
 */
static void test_strempty(void)
{
    ok(strempty(""), "strempty() handles well-formed empty string");
    ok(strempty(NULL), "strempty() handles NULL as an empty string");
    ok(!strempty("x"), "strempty() returns false for non-empty string");
}

/*
 * test_strtrunc() --strtrunc tests.
 */
static void test_strtrunc(void)
{
    char text[120];
    char text_result[120];
    char text100[] =
        DIGITS DIGITS DIGITS DIGITS DIGITS DIGITS DIGITS DIGITS DIGITS DIGITS;
    char *trunc_str;

    (void) estrcpy(text, "hello world");
    string_eq(strtrunc(text, 11, NULL), "hello world",
              "strtrunc() doesn't truncate if there is room");

    string_eq(strtrunc(text, 10, NULL), "hello w...",
              "strtrunc() truncates to fit specified length");

    string_eq(strtrunc(text, 3, NULL), "hel",
              "strtrunc() no room for ellipsis");

    (void) estrcpy(text, text100);
    string_eq(strtrunc(text, sizeof(text100), NULL),
              DIGITS DIGITS DIGITS DIGITS DIGITS
              DIGITS DIGITS DIGITS DIGITS "012345...",
              "strtrunc() handles internal limit buffer");

    (void) estrcpy(text, text100);
    string_eq((trunc_str = strtrunc(text, sizeof(text100), text_result)),
              DIGITS DIGITS DIGITS DIGITS DIGITS
              DIGITS DIGITS DIGITS DIGITS DIGITS,
              "strtrunc() handles explicit buffer");
    ptr_eq(trunc_str, text_result, "strtrunc() returns explicit buffer");
}

/*
 * test_strcpy() --strcpy tests.
 */
static void test_strcpy(void)
{
    char text[100] = "";
    char *end;

    end = estrcpy(estrcpy(text, "hello "), "world");
    number_eq(end - text, 11, "%d", "estrcpy() returns end of string");
    string_eq(text, "hello world", "estrcpy() string is copied");
}

/*
 * test_vstrcat() --vstrcat tests.
 */
static void test_vstrcat(void)
{
    char text[100] = "";
    char *end;

    end = vstrcat(text, "HELLO ", "WORLD", NULL);
    number_eq(end - text, 11, "%d", "vstrcat() returns end of string");
    string_eq(text, "HELLO WORLD", "vstrcat() concatenates string arguments");
    end = vstrcat(text, NULL);
    number_eq(end - text, 0, "%d", "vstrcat() empty list is a NOP");
}

/*
 * test_strtolower() --strtolower tests.
 */
static void test_strtolower(void)
{
    char text[100] = "";
    char *end;

    (void) estrcpy(text, "HELLO WORLD ***");
    end = estrtolower(text);
    string_eq(text, "hello world ***",
              "strtolower() translates to lowercase");
    number_eq(end, text + strlen(text), "%d",
              "estrtolower() returns end of string");
}

/*
 * test_strtoupper() --strtoupper tests.
 */
static void test_strtoupper(void)
{
    char text[100] = "";
    char *end;

    (void) estrcpy(text, "hello world ***");
    end = estrtoupper(text);
    string_eq(text, "HELLO WORLD ***",
              "estrtoupper() translates to UPPERCASE");
    number_eq(end, text + strlen(text), "%d",
              "estrtolower() returns end of stri");
}

/*
 * test_strsub() --strsub tests.
 */
static void test_strsub(void)
{
    char text[100] = "";
    char *end;

    (void) estrcpy(text, "hello world ***");
    end = estrsub(text, 'l', 'x', false);
    string_eq(text, "hexlo world ***",
              "estrsub() subtitutes single character first time");
    number_eq(end, text + strlen(text), "%d",
              "estrsub() returns end of string");

    end = estrsub(text, 'l', 'x', true);
    string_eq(text, "hexxo worxd ***", "estrsub() substitutes second time");
}

/*
 * test_strtr() --strtr tests.
 */
static void test_strtr(void)
{
    char text[100] = "";
    char *end;

    (void) estrcpy(text, "hello world ***");
    end = estrtr(text, "abcdefghijkl", "ABCDEFGHIJKL");
    string_eq(text, "HELLo worLD ***",
              "estrtr() translates all matching characters");
    number_eq(end, text + strlen(text), "%d",
              "estrtr() returns end of string");
}

/*
 * test_vstrmatch() --vstrmatch tests.
 */
static void test_vstrmatch(void)
{
    ok(vstrmatch("hello", "hello", NULL) == 0, "vstrmatch(): first item");
    ok(vstrmatch("hello", "workd", NULL) == -1, "vstrmatch(): not present");
    ok(vstrmatch("world", "hello", "world", NULL) == 1,
       "vstrmatch(): second item");
}

/*
 *  main() --Tests entrypoint.
 */
int main(void)
{
    plan_tests(26);

    test_strempty();
    test_strtrunc();
    test_strcpy();
    test_vstrcat();
    test_strtolower();
    test_strtoupper();
    test_strsub();
    test_strtr();
    test_vstrmatch();

    return exit_status();
}
