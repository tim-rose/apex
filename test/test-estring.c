/*
 * ESTRING.C --Unit tests for the estring module.
 */
#include <xtd/test.h>
#include <xtd/estring.h>

int main(void)
{
    char text[100] = "";
    char *end;

    plan_tests(18);

    ok(strempty(text), "strempty() empty string");

    end = estrcpy(estrcpy(text, "hello "), "world");

    ok(!strempty(text), "strempty() non-empty string");

    int_eq(end - text, 11, "%d", "estrcpy() string length");
    string_eq(text, "hello world", "estrcpy() string comparison");

    end = vstrcat(text, "HELLO ", "WORLD", NULL);
    int_eq(end - text, 11, "%d", "vstrcat() string length");
    string_eq(text, "HELLO WORLD", "vstrcat() string comparison");
    end = vstrcat(text, NULL);
    int_eq(end - text, 0, "%d", "vstrcat() empty list");

    (void) estrcpy(text, "hello world");
    string_eq(strtrunc(text, 11, NULL), "hello world",
              "strtrunc() no truncation");

    string_eq(strtrunc(text, 10, NULL), "hello w...",
              "strtrunc() truncation");

    string_eq(strtrunc(text, 3, NULL), "hel",
              "strtrunc() no room for ellipsis");

    (void) estrcpy(text, "HELLO WORLD ***");
    end = estrtolower(text);
    ok(strcmp(text, "hello world ***") == 0 &&
       end == text + strlen(text), "estrtolower()");

    end = estrtoupper(text);
    ok(strcmp(text, "HELLO WORLD ***") == 0 &&
       end == text + strlen(text), "estrtoupper()");

    (void) estrcpy(text, "hello world ***");
    end = estrsub(text, 'l', 'x', false);
    ok(strcmp(text, "hexlo world ***") == 0 &&
       end == text + strlen(text), "estrsub() once");

    end = estrsub(text, 'l', 'x', true);
    ok(strcmp(text, "hexxo worxd ***") == 0 &&
       end == text + strlen(text), "estrsub() all");

    (void) estrcpy(text, "hello world ***");
    end = estrtr(text, "abcdefghijkl", "ABCDEFGHIJKL");
    ok(strcmp(text, "HELLo worLD ***") == 0 &&
       end == text + strlen(text), "estrtr()");

    /*
     * vstrcmp tests.
     */
    ok(vstrmatch("hello", "hello", NULL) == 0, "vstrmatch(): first item");
    ok(vstrmatch("hello", "workd", NULL) == -1, "vstrmatch(): not present");
    ok(vstrmatch("world", "hello", "world", NULL) == 1,
       "vstrmatch(): second item");

    return exit_status();
}
