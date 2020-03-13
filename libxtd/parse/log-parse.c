/*
 * LOG-PARSE.C --Syslog parsing routines.
 *
 * Contents:
 * log_parse_r()       --Parse a syslog line, and return the parsed elements.
 * log_parse()         --Parse a syslog line, and return the parsed elements.
 * log_fgets()         --Read a line from a file, and parse it into a log-record.
 * decode_syslog_()    --decode a syslog text record into its pieces.
 * decode_timestamp_() --decode the timestamp part of a syslog message.
 * decode_ident_()     --decode the "<tag>[<pid>]" part of a syslog message.
 *
 * Remarks:
 * This module parses a single syslog text line into component pieces
 * (timestamp, host, ident, pid, message, etc.)
 *
 */
#include <string.h>

#include <xtd.h>
#include <strparse.h>
#include <symbol.h>
#include <date.h>
#include <log.h>
#include <sysenum.h>
#include <log-parse.h>

static LogRecordPtr decode_syslog_(LogRecordPtr log_record,
                                   struct tm *tm_base);
static char *decode_timestamp_(LogRecordPtr log_record, struct tm *tm_base);
static int decode_ident_(LogRecordPtr log_record, char *end);

static const char *ts_fmt[] = {        /* list of allowed timestamp formats */
    date_syslog_timestamp,
    0
};

/*
 * log_parse_r() --Parse a syslog line, and return the parsed elements.
 *
 * Parameters:
 * log_record   --specifies and returns a record for the parsed elements
 * str      --the string to parse as a syslog record
 * tm_base  --the base timestamp for decoding syslog's ambiguous format
 *
 * Returns: (LogRecordPtr)
 * Success: log_rec; Failure: NULL.
 */
LogRecordPtr log_parse_r(LogRecordPtr log_record, const char *str,
                         struct tm *tm_base)
{
    strncpy(log_record->text, str, sizeof(log_record->text));
    log_record->text[sizeof(log_record->text) - 1] = '\0';

    return decode_syslog_(log_record, tm_base);
}

/*
 * log_parse() --Parse a syslog line, and return the parsed elements.
 *
 * Parameters:
 * str      --the string to parse as a syslog record
 * tm_base  --the base timestamp for decoding syslog's ambiguous format
 *
 * Returns: (LogRecordPtr)
 * Success: an initialised log record; Failure: NULL.
 */
LogRecordPtr log_parse(const char *str, struct tm * tm_base)
{
    static LogRecord log_record;

    return log_parse_r(&log_record, str, tm_base);
}

/*
 * log_fgets() --Read a line from a file, and parse it into a log-record.
 *
 * Parameters:
 * log_record   --specifies and returns a record for the parsed elements
 * fp       --a file pointer open for reading
 * tm_base  --the base timestamp for decoding syslog's ambiguous format
 *
 * Returns: (LogRecordPtr)
 * Success: an initialised log record; Failure: NULL.
 *
 * Remarks:
 * This function conflates the error cases for fgets() and decode_syslog_().
 */
LogRecordPtr log_fgets(LogRecordPtr log_record, FILE * fp,
                       struct tm * tm_base)
{
    if (fgets(log_record->text, sizeof(log_record->text), fp) != NULL)
    {
        char *eol = strchr(log_record->text, '\n');

        if (eol != NULL)
        {
            *eol = '\0';
        }
        else
        {
            log_record->text[sizeof(log_record->text) - 1] = '\0';
        }
        return decode_syslog_(log_record, tm_base);
    }
    return NULL;                       /* error: fgets() failed */
}

/*
 * decode_syslog_() --decode a syslog text record into its pieces.
 *
 * Parameters:
 * log_record   --specifies and returns the syslog record
 * tm_base  --the base timestamp for decoding syslog's ambiguous format
 *
 * Returns: (LogRecordPtr)
 * Success: an initialised log record; Failure: NULL.
 *
 * Remarks:
 * On entry, the log_record->text contains the raw text to be parsed.
 * On successful return, the other fields will be initialised with
 * fragments of the raw text.
 */
static LogRecordPtr decode_syslog_(LogRecordPtr log_record,
                                   struct tm *tm_base)
{
    char *str;
    char *end;
    char pri_text[10];
    struct tm log_base = *tm_base;

    log_base.tm_isdst = -1;            /* ignore tm_base's dst */

    log_record->pid = -1;
    log_record->facility = -1;
    log_record->priority = -1;

    if ((str = decode_timestamp_(log_record, &log_base)) == NULL)
    {
        return NULL;
    }
    log_record->host = str;

    if ((str = strchr(log_record->host, ':')) == NULL)
    {
        return NULL;
    }
    *str++ = '\0';
    if (!decode_ident_(log_record, str - 1))
    {
        return NULL;
    }
    while (*str == ' ')
    {
        ++str;                         /* skip whitespace */
    }
    log_record->message = str;

    /*
     * try to parse an initial "priority:" text in the message part.
     * Note that we assume syslog_priority is a null-terminated list,
     * and rely on str_enum() detecting/bailing before 100 items(!).
     */
    strncpy(pri_text, log_record->message, sizeof(pri_text));
    pri_text[sizeof(pri_text) - 1] = '\0';
    if ((end = strchr(pri_text, ':')) != NULL)
    {                                  /* possibly "priority: xxx..." */
        *end++ = '\0';
        if (str_enum
            (pri_text, 100, (EnumPtr) syslog_priority, &log_record->priority))
        {
            log_record->message += (end - pri_text);
            while (*log_record->message == ' ')
            {
                ++log_record->message; /* skip whitespace */
            }
        }
    }
    return log_record;
}

/*
 * decode_timestamp_() --decode the timestamp part of a syslog message.
 */
static char *decode_timestamp_(LogRecordPtr log_record, struct tm *tm_base)
{
    struct tm tm_syslog = *tm_base;
    char *str;

    if ((str = (char *)
         date_parse_fmt(log_record->text, &tm_syslog,
                        ts_fmt, 1, NULL)) == NULL)
    {
        return NULL;                   /* error: bad timestamp */
    }
    log_record->timestamp = mktime(&tm_syslog);

    while (*str == ' ')
    {
        ++str;                         /* skip whitespace */
    }
    return str;                        /* success */
}

/*
 * decode_ident_() --decode the "<tag>[<pid>]" part of a syslog message.
 *
 * Remarks:
 * On entry, log_record->host contains the host (and possibly) the identity
 * component.
 */
static int decode_ident_(LogRecordPtr log_record, char *end)
{
    char *str;
    log_record->tag = end--;

    if ((str = strchr(log_record->host, ' ')) == NULL)
    {
        return 1;                      /* success: (no tag) */
    }
    *str++ = '\0';                     /* truncate host part */
    log_record->tag = str;

    if (*end == ']')
    {                                  /* looks like "tag[pid]" */
        char *start = strrchr(log_record->tag, '[');

        if (start != NULL)
        {
            int pid;

            *start++ = '\0';           /* terminate tag */
            *end = '\0';
            if (!str_int(start, &pid))
            {
                return 0;              /* error: can't decipher pid */
            }
            log_record->pid = pid;
        }
    }
    return 1;
}
