/*
 * LOG-PARSE.C --Unit tests for syslog parsing.
 *
 * Contents:
 * logrec_cmp_()    --Compare two log records for equality.
 * logrec_sprint_() --Print a log record.
 * main()           --Run some unit tests.
 *
 *
 */
#include <string.h>
#include <time.h>

#include <test.h>
#include <xtd.h>
#include <log-parse.h>

#define logrec_eq(_l, _l_ref, ...) \
    object_eq(_l, _l_ref, logrec_cmp_, logrec_sprint_, __VA_ARGS__)

/*
 * logrec_cmp_() --Compare two log records for equality.
 */
static int logrec_cmp_(LogRecordPtr lr1, LogRecordPtr lr2)
{
    if (lr1->timestamp != lr2->timestamp
        || lr1->pid != lr2->pid
        || lr1->facility != lr2->facility || lr1->priority != lr2->priority)
    {
        return -1;                     /* different! */
    }
    if (strcmp(lr1->host, lr2->host) != 0
        || strcmp(lr1->tag, lr2->tag) != 0
        || strcmp(lr1->message, lr2->message) != 0)
    {
        return -1;                     /* different! */
    }
    return 0;                          /* success: (0 differences) */
}

/*
 * logrec_sprint_() --Print a log record.
 *
 * Remarks:
 * This routine returns results in an alternating set of of static
 * buffers, because the object_eq() macro will call this function
 * twice, and must get distinct strings to feed to a subsequent
 * diag().
 */
static const char *logrec_sprint_(LogRecordPtr lr)
{
    static int slot = 0;
    static char str_buf[2][LINE_MAX];

    slot = (slot + 1) % 2;
    if (lr == NULL)
    {
        return STR_OR_NULL(NULL);
    }
    sprintf(str_buf[slot],
            "{ "
            "timestamp=%d, "
            "host=\"%s\", "
            "tag=\"%s\", "
            "pid=%d, "
            "facility=%d, "
            "priority=%d, "
            "message=\"%s\" }",
            (int) lr->timestamp,
            STR_OR_NULL(lr->host),
            STR_OR_NULL(lr->tag),
            lr->pid, lr->facility, lr->priority, STR_OR_NULL(lr->message));
    return str_buf[slot];
}

/*
 * main() --Run some unit tests.
 *
 * Remarks:
 * One of the "features" of syslog-formatted dates is that they lack
 * a year spec (although some modern syslogs have configurable formats).
 * This is handled by passing a "representative" date in the same year.
 */
int main(void)
{
    LogRecordPtr l;
    struct tm base_tm = {.tm_isdst = -1 }, feb_tm = {
        .tm_isdst = -1
    };
    time_t feb_ut, now_ut;

    /* setup reference timestamp as an epoch */
    strptime("2001-02-01 00:00:00", "%Y-%m-%d %H:%M:%S", &feb_tm);
    feb_ut = mktime(&feb_tm);

    /* setup "base" timeframe => 2001 */
    time(&now_ut);
    localtime_r(&now_ut, &base_tm);
    base_tm.tm_year = 101;             /* rewind to same day in 2001 */

    plan_tests(12);

    do
    {                                  /* simple, common case */
        LogRecord lr_base = {
            feb_ut, (char *) "host", (char *) "tag",
            100, -1, 6, (char *) "hello world", {0}
        };
        l = log_parse("Feb 01 00:00:00 host tag[100]: info: hello world",
                      &base_tm);

        ok(l != NULL, "valid record parses OK")
            && logrec_eq(l, &lr_base, "valid values");
    } while (0);

    do
    {                                  /* simplest possible valid record */
        LogRecord lr_base = {
            feb_ut, (char *) "", (char *) "",
            -1, -1, -1, (char *) "hello world", {0}
        };
        l = log_parse("Feb 01 00:00:00 : hello world", &base_tm);

        ok(l != NULL, "minimal record parses OK")
            && logrec_eq(l, &lr_base, "minimal record values");
    } while (0);

    do
    {                                  /* record with hostname only */
        LogRecord lr_base = {
            feb_ut, (char *) "host", (char *) "",
            -1, -1, -1, (char *) "hello world", {0}
        };
        l = log_parse("Feb 01 00:00:00 host: hello world", &base_tm);

        ok(l != NULL, "hostname-only record parses OK")
            && logrec_eq(l, &lr_base, "hostname-only record values");
    } while (0);

    do
    {                                  /* record with hostname, tag only */
        LogRecord lr_base = {
            feb_ut, (char *) "host", (char *) "tag",
            -1, -1, -1, (char *) "hello world", {0}
        };
        l = log_parse("Feb 01 00:00:00 host tag: hello world", &base_tm);

        ok(l != NULL, "host+tag record parses OK")
            && logrec_eq(l, &lr_base, "host+tag record values");
    } while (0);

    do
    {                                  /* record without a priority */
        LogRecord lr_base = {
            feb_ut, (char *) "", (char *) "",
            -1, -1, -1, (char *) "not-a-priority: hello world", {0}
        };
        l = log_parse("Feb 01 00:00:00 : not-a-priority: hello world",
                      &base_tm);

        logrec_eq(l, &lr_base, "ignore non-priority message prefix");
    } while (0);

    /*
     * check various error return causes.
     */
    ok(log_parse("Xxx 01 00:00:00 : bad timestamp", &base_tm) == NULL,
       "Return NULL for bad timestamp");
    ok(log_parse("Feb 01 00:00:00 bad-ident", &base_tm) == NULL,
       "Return NULL for bad identity (no colon)");
    ok(log_parse("Feb 01 00:00:00 ident[xxx]", &base_tm) == NULL,
       "Return NULL for bad identity (invalid pid)");

    return exit_status();
}
