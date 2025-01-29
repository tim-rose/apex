/*
 * DATE.C --Unit tests for the date functions.
 *
 * Contents:
 * timestamp_eq() --Test/report that timestamps are equal.
 * sprint_ut()    --Format a time_t value to a string.
 * parse_test()   --Run a single date-parsing test.
 * tv_tests()     --Tests for timeval operations.
 * main()         --Run some date unit tests.
 */
#include <string.h>
#include <apex/test.h>
#include <apex.h>
#include <apex/date.h>
#include <apex/timeval.h>

#ifdef __WINNT__
#define localtime_r(timep_, result_) localtime(timep_)
#endif /* __WINNT__ */

/*
 * parse_test() --Run a single date-parsing test.
 *
 * Parameters:
 * text     --the text to be parsed
 * ref_t    --the reference timestamp
 * description  --the test description
 *
 * Remarks:
 * Some calls will have ambiguous text; we initialise the buffer
 * with a date that matches the year of the test values.
 */
static void parse_test(const char *text, time_t * ref_t,
                       const char *description)
{
    time_t t;
    struct tm t_tm = {.tm_year = 110,.tm_hour = 12,.tm_isdst = -1 };

    ptr_eq(date_parse_timestamp(text, &t_tm, &t), (text + strlen(text)),
       "%s: parse", description);
    number_eq(t, *ref_t, "%ld", "%s: values", description);
}

/*
 * tv_tests() --Tests for timeval operations.
 */
static void tv_tests(void)
{
    do
    {
        TimeValue t1 = { 1, 1500000 };

        tv_normalise(&t1);
        number_eq(t1.tv_sec, 2, "%d", "tv_normalise: adjust up (sec)");
        number_eq(t1.tv_usec, 500000, "%d", "tv_normalise: adjust up (usec)");
    } while (0);

    do
    {
        TimeValue t1 = { 2, -1500000 };

        tv_normalise(&t1);
        number_eq(t1.tv_sec, 0, "%d", "tv_normalise: adjust down (sec)");
        number_eq(t1.tv_usec, 500000, "%d",
                  "tv_normalise: adjust down (usec)");
    } while (0);

    do
    {
        TimeValue t1 = { 2, 1000000 };
        TimeValue t2 = { 1, 1000000 };
        TimeValue t_diff;

        tv_diff_r(&t1, &t2, &t_diff);
        number_eq(t_diff.tv_sec, 1, "%d", "tv_diff: (sec)");
        number_eq(t_diff.tv_usec, 0, "%d", "tv_diff: (usec)");
    } while (0);

    do
    {
        TimeValue t1 = { 2, 1000000 };
        TimeValue t2 = { 1, 1000000 };
        TimeValue t_sum;

        tv_sum_r(&t1, &t2, &t_sum);
        number_eq(t_sum.tv_sec, 5, "%d", "tv_sum: (sec)");
        number_eq(t_sum.tv_usec, 0, "%d", "tv_sum: (usec)");
    } while (0);

    do
    {
        TimeValue t1 = { 2, 1000000 };
        TimeValue t2 = { 1, 1000000 };

        tv_sum_r(&t1, &t2, &t1);
        number_eq(t1.tv_sec, 5, "%d", "tv_sum/t1==sum: (sec)");
        number_eq(t1.tv_usec, 0, "%d", "tv_sum/t1==sum: (usec)");
    } while (0);

    do
    {
        TimeValue t1 = { 1, 0000000 };

        tv_scale_r(&t1, 3.14159265, &t1);
        number_eq(t1.tv_sec, 3, "%d", "tv_mul: (sec)");
        number_eq(t1.tv_usec, 141592, "%d", "tv_sum: (usec)");
    } while (0);
}

/*
 * main() --Run some date unit tests.
 *
 * Remarks:
 * These tests assume that we're running on the East coast
 * of Australia, EST+10/EDT+11.  They create two "target" dates
 * in the winter/summer, and then form various date spec.s that
 * should match it.
 *
 * The reference dates were derived independently by:
 * * date -d '2010-01-01 12:00:00' '+%s'
 * * date -d '2010-07-01 12:00:00' '+%s'
 */
int main(void)
{
    time_t jul_t, jan_t;
    struct tm jul_tm = null_tm;
    struct tm jan_tm = null_tm;

    jan_t = 1262307600;                /* 2010-01-01 12:00:00 EST */
    localtime_r(&jan_t, &jan_tm);

    jul_t = 1277949600;                /* 2010-07-01 12:00:00 EST */
    localtime_r(&jul_t, &jul_tm);

    plan_tests(42);

    parse_test("2010-07-01", &jul_t, "%Y-%m-%d");
    parse_test("20100701", &jul_t, "%Y%m%d");

    parse_test("Jul 1", &jul_t, "%b %e");
    parse_test("July 1", &jul_t, "%B %e");

    jul_t += 7200;                     /* change time by two hours */
    parse_test("Jul 1 14:00:00", &jul_t, "syslog");
    parse_test("2010-07-01T14:00:00", &jul_t, "ISO8601(T)");
    parse_test("2010-07-01 14:00:00", &jul_t, "ISO8601( )");

    strptime("2010-07-01 12:00:00", "%Y-%m-%d %H:%M:%S", &jul_tm);
    jul_t = mktime(&jul_tm);
    todo_start("Linux can't parse \"Z\" timezone");
    parse_test("2010-07-01 02:00:00Z", &jul_t, "Jul ISO8601(Z)");
    parse_test("2010-01-01 01:00:00Z", &jan_t, "Jan ISO8601(Z)");
    todo_end();

    parse_test("2010-W26-4", &jul_t, "%Y-W%W-%w");
    parse_test("2010W264", &jul_t, "%YW%W%w");

    parse_test("2010-182 12:00:00", &jul_t, "%Y-%j");
    parse_test("2010-001 12:00:00", &jan_t, "%Y-%j");

    strptime("2010-01-02 12:00:00", "%Y-%m-%d %H:%M:%S", &jan_tm);
    jan_t = mktime(&jan_tm);
    parse_test("2010-002 12:00:00", &jan_t, "%Y-%j");

    parse_test("@1277949600", &jul_t, "%s");

    tv_tests();
    return exit_status();
}
