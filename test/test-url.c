/*
 * URL.C --Unit tests for the URL functions.
 *
 * Contents:
 * test_str_url() --Test the behaviour of str_url().
 * test_encode()  --Test the behaviour of url_encode().
 *
 */
#include <string.h>

#include <xtd/test.h>
#include <xtd.h>
#include <xtd/url.h>

static void test_str_url(void);
static void test_encode(void);

int main(void)
{
    plan_tests(35);

    test_str_url();
    test_encode();

    return exit_status();
}

/*
 * test_str_url() --Test the behaviour of str_url().
 */
static void test_str_url(void)
{
    char buf[NAME_MAX];

    do
    {
        char str[] = "domain/path";
        URL url = {.scheme = (char *) "http",.port = 80 };
        char description[] = "simple URL";

        ok(str_url(str, &url), "str_url(%s)", description);

        str_field_eq(url, scheme, "http", "str_url(%s) default", description);
        str_field_eq(url, user, NULL, "str_url(%s) unset", description);
        str_field_eq(url, password, NULL, "str_url(%s) unset", description);
        str_field_eq(url, domain, "domain", "str_url(%s)", description);
        int_field_eq(url, port, 80, "str_url(%s) default", description);
        str_field_eq(url, path, "path", "str_url(%s)", description);
        str_field_eq(url, query, NULL, "str_url(%s) unset", description);
        str_field_eq(url, anchor, NULL, "str_url(%s) unset", description);

        snprint_url(buf, NEL(buf), &url);
        string_eq(buf, "http://domain:80/path", "snprint_url(%s)",
                  description);
    } while (0);

    do
    {
        char str[] = "http://user:pass@domain:80/path?query#anchor";
        char description[] = "fully-specified URL";
        URL url = null_url;

        ok(str_url(str, &url), "str_url(%s)", description);

        str_field_eq(url, scheme, "http", "str_url(%s)", description);
        str_field_eq(url, user, "user", "str_url(%s)", description);
        str_field_eq(url, password, "pass", "str_url(%s)", description);
        str_field_eq(url, domain, "domain", "str_url(%s)", description);
        int_field_eq(url, port, 80, "str_url(%s)", description);
        str_field_eq(url, path, "path", "str_url(%s)", description);
        str_field_eq(url, query, "query", "str_url(%s)", description);
        str_field_eq(url, anchor, "anchor", "str_url(%s)", description);

        snprint_url(buf, NEL(buf), &url);
        string_eq(buf, "http://user:pass@domain:80/path?query#anchor",
                  "snprint_url(%s)", description);
    } while (0);

    do
    {
        char str[] = "mailto:user@domain";
        char description[] = "mailto URL";
        URL url = null_url;

        ok(str_url(str, &url), "str_url(%s)", description);

        str_field_eq(url, scheme, "mailto", "str_url(%s)", description);
        str_field_eq(url, user, "user", "str_url(%s)", description);
        str_field_eq(url, domain, "domain", "str_url(%s)", description);
        int_field_eq(url, port, 25, "str_url(%s)", description);

        snprint_url(buf, NEL(buf), &url);
        string_eq(buf, "mailto:user@domain:25", "snprint_url(%s)",
                  description);
    } while (0);

    do
    {
        char str[] = "domain:80";
        char description[] = "domain+port";
        URL url = null_url;

        ok(str_url(str, &url), "str_url(%s)", description);

        str_field_eq(url, domain, "domain", "str_url(%s)", description);
        int_field_eq(url, port, 80, "str_url(%s)", description);

        snprint_url(buf, NEL(buf), &url);
        string_eq(buf, "domain:80", "snprint_url(%s)", description);
    } while (0);
}

/*
 * test_encode() --Test the behaviour of url_encode().
 */
static void test_encode(void)
{
    char buf[1024];
    const char *test = "The-quick-brown-fox-jumps-over-the-lazy-dog";
    size_t n = url_encode(test, NEL(buf), buf);

    int_eq(n, strlen(test), "%d", "url_encode(trivial): return value");

    ok(buf[n] == '\0', "url_encode(trivial): terminates copied string");
    string_eq(test, buf, "url_encode(trivial): copied text");

    n = url_encode("$&+,/:;=?@", NEL(buf), buf);

    int_eq(n, 30, "%d", "url_encode(reserved): return value");
    string_eq("%24%26%2b%2c%2f%3a%3b%3d%3f%40", buf,
              "url_encode(reserved): copied text");
    /* REVISIT: boundary tests */
    /* REVISIT: error tests */
}
