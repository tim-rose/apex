/*
 * SYMBOL.C --symbol lookup tests.
 *
 * Contents:
 * sym_get_test() --Test sym_path/sym_get.
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <float.h>

#include <xtd/tap.h>
#include <xtd.h>
#include <xtd/vector.h>
#include <xtd/symbol.h>

/*
 * The dom is equivalent to the interned JSON for:
 * > a=1
 * > b=2.0
 * > c="foobar"
 * > a_list {
 * >     1 2.0 "foobar"
 * > }
 * > a_struct {
 * >     a=1
 * >     b=2.0
 * >     c="foobar"
 * > }
 */
Atom a_list[] = {
    {.type = INTEGER_TYPE,.value.integer = 1},
    {.type = REAL_TYPE,.value.real = 2.0},
    {.type = STRING_TYPE,.value.string = (char *) "foobar"},
    NULL_ATOM
};

Symbol a_struct[] = {
    {.name = (char *) "a",.type = INTEGER_TYPE,.value.integer = 1},
    {.name = (char *) "b",.type = REAL_TYPE,.value.real = 2.0},
    {.name = (char *) "c",.type = STRING_TYPE,.value.string = (char *) "foobar"},
    NULL_SYMBOL
};

Symbol test_dom[] = {
    {.name = (char *) "a",.type = INTEGER_TYPE,.value.integer = 1},
    {.name = (char *) "b",.type = REAL_TYPE,.value.real = 2.0},
    {.name = (char *) "c",.type = STRING_TYPE,.value.string = (char *) "foobar"},
    {.name = (char *) "a_list",.type = LIST_TYPE,.value.list = a_list},
    {.name = (char *) "a_struct",.type = STRUCT_TYPE,.value.field = a_struct},
    NULL_SYMBOL
};

/*
 * sym_get_test() --Test sym_path/sym_get.
 *
 * Remarks:
 * The dom is equivalent to the interned JSON for:
 * "a=1 b=2.0 a_list { 1 2.0 "foobar" } a_struct { a=1 b=2.0 }"
 */
static void sym_test(SymbolPtr dom, const char *pathname,
                     Type t, Value v, const char *comment)
{
    Value val;
    AtomPtr path = new_sym_path(pathname);
    int cmp;


    ok(path != NULL, "sym_path(): %s", comment);
    /* TODO: verify structure of path */
    ok(path != NULL &&
       sym_get_value(dom, path, &val) == t, "sym_get: %s type", comment);
    switch (t)
    {
    case INTEGER_TYPE:
        cmp = val.integer == v.integer;
        break;
    case REAL_TYPE:
        cmp = val.real == v.real;
        break;
    case STRING_TYPE:
        cmp = strcmp(val.string, v.string) == 0;
        break;
    case VOID_TYPE:
    case LIST_TYPE:
    case STRUCT_TYPE:
    default:
        cmp = 1;
        break;
    }
    ok(path != NULL && cmp, "sym_get: %s value", comment);
    free_sym_path(path);
}

/*
 * main...
 */
int main(void)
{
    Value v;

    plan_tests(33);

    ok(new_sym_path(NULL) == NULL, "NULL path returns NULL");

    do
    {
        Atom ref_path[] = {
            {.type = STRING_TYPE,.value.string = (char *) "a"},
            NULL_ATOM
        };
        AtomPtr path = new_sym_path("a");

        ok(path != NULL && sym_path_equal(path, ref_path),
           "parse simple identifier");
        free_sym_path(path);
    } while (0);

    do
    {
        Atom ref_path[] = {
            {.type = STRING_TYPE,.value.string = (char *) "a"},
            {.type = STRING_TYPE,.value.string = (char *) "b"},
            {.type = STRING_TYPE,.value.string = (char *) "c"},
            NULL_ATOM
        };
        AtomPtr path = new_sym_path("a.b.c");

        ok(path != NULL && sym_path_equal(path, ref_path),
           "parse struct path");
        free_sym_path(path);
    } while (0);

    do
    {
        Atom ref_path[] = {
            {.type = INTEGER_TYPE,.value.integer = 1}
            ,
            NULL_ATOM
        };
        AtomPtr path = new_sym_path("[1]");

        ok(path != NULL && sym_path_equal(path, ref_path),
           "parse simple array ref");
        free_sym_path(path);
    } while (0);

    do
    {
        Atom ref_path[] = {
            {.type = STRING_TYPE,.value.string = (char *) "foo"},
            {.type = INTEGER_TYPE,.value.integer = 100},
            NULL_ATOM
        };
        AtomPtr path = new_sym_path("foo[100]");

        ok(path != NULL && sym_path_equal(path, ref_path),
           "parse struct+array ref");
        free_sym_path(path);
    } while (0);

    do
    {
        Atom ref_path[] = {
            {.type = STRING_TYPE,.value.string = (char *) "foo"},
            {.type = INTEGER_TYPE,.value.integer = 100},
            {.type = STRING_TYPE,.value.string = (char *) "bar"},
            NULL_ATOM
        };
        AtomPtr path = new_sym_path("foo[100].bar");

        ok(path != NULL && sym_path_equal(path, ref_path),
           "parse struct+array+struct ref");
        free_sym_path(path);
    } while (0);

    v.integer = 1;
    sym_test((SymbolPtr) & test_dom, "a", INTEGER_TYPE, v, "simple path to int");
    sym_test((SymbolPtr) & test_dom, "a_list[0]", INTEGER_TYPE, v,
             "array syntax to int");
    sym_test((SymbolPtr) & test_dom, "a_struct.a", INTEGER_TYPE, v,
             "struct syntax to int");

    v.real = 2.0;
    sym_test((SymbolPtr) & test_dom, "b", REAL_TYPE, v, "simple path to real");
    sym_test((SymbolPtr) & test_dom, "a_list[1]", REAL_TYPE, v,
             "array syntax to real");
    sym_test((SymbolPtr) & test_dom, "a_struct.b", REAL_TYPE, v,
             "struct syntax to real");

    v.string = (char *) "foobar";
    sym_test((SymbolPtr) & test_dom, "c", STRING_TYPE, v, "simple path to string");
    v.string = (char *) "foobar";
    sym_test((SymbolPtr) & test_dom, "a_list[2]", STRING_TYPE, v,
             "array syntax to string");
    sym_test((SymbolPtr) & test_dom, "a_struct.c", STRING_TYPE, v,
             "struct syntax to string");
    return exit_status();
}
