/*
 * SYSTOOLS.C --Unit tests for the "systools" functions.
 *
 * Contents:
 * test_make_path() --Unit tests for make_path().
 * test_link_path() --Unit tests for link_path().
 * test_dirname()   --Unit tests for path_basename(), path_dirname().
 *
 * Remarks:
 * Actually, all I'm testing at the moment is the make_path() function,
 * because the others seem too trivial...
 *
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <apex/tap.h>
#include <apex/test.h>
#include <apex.h>
#include <apex/systools.h>
#include <apex/log.h>

/*
 * test_make_path() --Unit tests for make_path().
 */
static void test_make_path(void)
{
    char path[FILENAME_MAX];
    char cmd[FILENAME_MAX];
    char *root = getenv("TMPDIR");
    struct stat stat_buf;

    if (root == NULL)
    {
        root = (char *) ".";
    }
    sprintf(path, "%s/%s", root, "a");
    ok(make_path(path), "simple path");
    ok(stat(path, &stat_buf) == 0
       && stat_buf.st_mode & S_IFDIR,
       "make_path: simple path, directory exists");

    chmod(path, 0400);                 /* make it readonly */
    sprintf(path, "%s/%s", root, "a/b/c/d");
    ok(!make_path(path), "make_path: protected path fails");

    sprintf(path, "%s/%s", root, "a");
    chmod(path, 0700);                 /* restore permissions */
    ok(make_path(path), "make_path: complex path"); /* mkdir should work this time */
    ok(stat(path, &stat_buf) == 0
       && stat_buf.st_mode & S_IFDIR,
       "make_path: complex path, directory exists");

    /* cleanup... */
    sprintf(cmd, "/bin/rm -rf %s/%s", root, "a");
    int ignore = system(cmd);

    sprintf(cmd, "%s/%s", root, "a");
    touch(cmd);
    sprintf(path, "%s/%s", root, "a");
    ok(!make_path(path),
       "make_path: simple path blocked by file of same name");
    sprintf(path, "%s/%s", root, "a/b/c/d");
    ok(!make_path(path),
       "make_path: complex path blocked by file of same name");

    /* cleanup... */
    sprintf(cmd, "/bin/rm -rf %s/%s", root, "a");
    ignore = system(cmd);
    (void) ignore;
}


/*
 * test_link_path() --Unit tests for link_path().
 */
static void test_link_path(void)
{
    char path[FILENAME_MAX];
    char path2[FILENAME_MAX];
    char cmd[FILENAME_MAX];
    char *root = getenv("TMPDIR");

    if (root == NULL)
    {
        root = (char *) ".";
    }

    sprintf(path, "%s/%s", root, "a");
    sprintf(path2, "%s/%s", root, "b");
    link_path(path, path2);
    ok(!link_path(path, path2), "link_path: src doesn't exist");
    touch(path);
    ok(link_path(path, path2), "link_path: success");
    ok(!link_path(path, path2), "link_path: dst already exists");

    /* cleanup... */
    sprintf(cmd, "/bin/rm -rf %s/%s", root, "a");
    int ignore = system(cmd);
    sprintf(cmd, "/bin/rm -rf %s/%s", root, "b");
    ignore = system(cmd);
    (void) ignore;
}


/*
 * test_dirname() --Unit tests for path_basename(), path_dirname().
 */
static void test_dirname(void)
{
    const char *empty = "";
    char dir[FILENAME_MAX + 1];

    ok(path_basename(empty) == empty, "path_basename: empty string");
    string_eq(path_basename("xyzzy"), "xyzzy",    /* not "./xyzzy" */
       "path_basename: no directory component");
    string_eq(path_basename("./xyzzy"), "xyzzy",
       "path_basename: relative directory component");
    string_eq(path_basename("/a/b/c/d/xyzzy"), "xyzzy",
       "path_basename: absolute directory component");

    ok(path_dirname("xyzzy", 0, dir) == NULL,
       "path_dirname: no space in directory buffer");
    string_eq(path_dirname("xyzzy", NEL(dir), dir), ".",
       "path_dirname: no directory component");
    string_eq(path_dirname("./xyzzy", NEL(dir), dir), ".",
       "path_dirname: relative directory component");
    string_eq(path_dirname("/a/b/c/d/xyzzy", NEL(dir), dir), "/a/b/c/d",
       "path_dirname: absolute directory component");
}

int main(void)
{
    plan_tests(18);
    test_make_path();
    test_link_path();
    test_dirname();
    return exit_status();
}
