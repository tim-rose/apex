/*
 * DATE.C --Date and time manipulation and parsing functions.
 *
 * Contents:
 * timegm_()            --Process a tm struct as if it was a UTC time spec.
 * date_parse_fmt()     --Parse (part of) a timestamp using a list of formats.
 * date_parse_date()    --Parse the "date" part only of a timestamp.
 * date_parse_time()    --Parse the "time" part only of a timestamp.
 * date_parse_timestamp() --Parse a timestamp.
 * fmt_time()           --Format a time_t value using strftime().
 *
 * Remarks:
 * This module attempts to provide a reliable way of parsing some
 * common formats that are defined by ISO8601, and that can be
 * processed via strptime().
 *
 * See Also:
 * http://en.wikipedia.org/wiki/ISO_8601
 *
 * REVISIT: implement RFC-3339: "%Y-%m-%d %H:%M:%S-hh:mm"
 * (e.g. 2006-08-07 12:34:56-06:00)
 */
#include <apex.h>
#include <apex/date.h>
#include <apex/estring.h>

#ifdef __WINNT__
#define localtime_r(timep_, result_) localtime(timep_)
#endif /* __WINNT__ */

struct tm null_tm = {.tm_isdst = -1 };

const char date_syslog_timestamp[] = "%b %e %H:%M:%S";
const char date_ISO8601_timestamp[] = "%Y-%m-%dT%H:%M:%S%z";
const char date_ISO8601_date[] = "%Y-%m-%d";    /* == %F */
const char date_ISO8601_time[] = "%H:%M:%S";    /* == %T */

static const char *date_date_list[] = {
    date_ISO8601_date,
    "%Y%m%d",
    "%b %e",
    "%B %e"
};

static const char *date_jdate_list[] = {
    "%Y-W%W-%w",                       /* NOTE: Doesn't work on Darwin */
    "%YW%W%w",
    "%Y-%j",
    "%Y%j"
};

static const char *date_time_list[] = {
    "%H:%M:%S%z",
    date_ISO8601_time,
    "%H%M%S%z",
    "%H%M%S",
};

/*
 * timegm_() --Process a tm struct as if it was a UTC time spec.
 *
 * Remarks:
 * This function is copied/adapted from the GNU timegm(3) manual page.
 * Note that this is a *private* implementation,
 */
static time_t timegm_(struct tm *tm)
{
    time_t t = 0;

#ifdef __WINNT__                       /* Windows doesn't have gmtoff */
    {
        char *tz;

        tz = getenv("TZ");             /* save current TZ, set to UTC */
        setenv("TZ", "", 1);
        tzset();
        t = mktime(tm);
        if (tz != NULL)                /* revert original TZ state */
            setenv("TZ", tz, 1);
        else
            unsetenv("TZ");
        tzset();
    }
#else
    t = mktime(tm);
    t += tm->tm_gmtoff;
#endif /* WINNT */
    return t;
}

/*
 * date_parse_fmt() --Parse (part of) a timestamp using a list of formats.
 *
 * Parameters:
 * text     --the text to parse
 * base_tm  --specifies a base timestamp, returns the parsed timestamp
 * fmt      --a list of formats to try
 * n_fmt    --the number of elements in the format list
 * match_fmt    --returns the format that matched
 *
 * Returns:
 * Same as strptime(3).
 *
 * Remarks:
 * This routine returns the first format that succeeds.
 */
const char *date_parse_fmt(const char *text, struct tm *base_tm,
                           const char *fmt[], size_t n_fmt,
                           const char **match_fmt)
{
    for (size_t i = 0; i < n_fmt; ++i)
    {
        struct tm t_tm = *base_tm;
        const char *parse_end;

        if ((parse_end = strptime(text, fmt[i], &t_tm)) != NULL)
        {
            if (match_fmt != NULL)
            {
                *match_fmt = fmt[i];   /* remember matched format for caller */
            }
            *base_tm = t_tm;           /* remember the parsed values */
            base_tm->tm_isdst = -1;    /* force recalculation of dst */
            return (const char *) parse_end;    /* success */
        }
    }
    return (const char *) NULL;        /* failure: nothing matched */
}

/*
 * date_parse_date() --Parse the "date" part only of a timestamp.
 *
 * Parameters:
 * text     --the text to parse
 * base_tm  --specifies and returns the timestamp
 *
 * Returns:
 * Same as strptime(3).
 */
const char *date_parse_date(const char *text, struct tm *base_tm, time_t * t)
{
    const char *fmt;
    const char *parse_end;

    if ((parse_end = date_parse_fmt(text, base_tm, date_date_list,
                                    NEL(date_date_list), &fmt)) != NULL)
    {
        *t = mktime(base_tm);
        return parse_end;
    }
    if ((parse_end = date_parse_fmt(text, base_tm, date_jdate_list,
                                    NEL(date_date_list), &fmt)) != NULL)
    {
#ifdef __Darwin__                      /* compensate for Darwin strptime(%j) bug */
        int yday = base_tm->tm_yday + 1;

        base_tm->tm_mon = yday / 28;
        base_tm->tm_mday = yday % 28;
        *t = mktime(base_tm);
        base_tm->tm_mday += yday - base_tm->tm_yday - 1;
#endif /* __Darwin__ */
        *t = mktime(base_tm);
    }
    return parse_end;
}

/*
 * date_parse_time() --Parse the "time" part only of a timestamp.
 *
 * Parameters:
 * text     --the text to parse
 * base_tm  --specifies and returns the timestamp
 *
 * Returns:
 * Same as strptime(3).
 */
const char *date_parse_time(const char *text, struct tm *base_tm, time_t * t)
{
    const char *fmt;
    const char *parse_end;

    parse_end = date_parse_fmt(text, base_tm,
                               date_time_list, NEL(date_time_list), &fmt);

    if (parse_end != NULL)
    {
        if (*parse_end == 'Z')
        {
            *t = timegm_(base_tm);
            parse_end += 1;
        }
        else
        {
            *t = mktime(base_tm);
        }
    }
    return parse_end;
}

/*
 * date_parse_timestamp() --Parse a timestamp.
 *
 * Parameters:
 * text     --the text to parse
 * base_tm  --specifies the base timestamp
 *
 * Returns:
 * Same as strptime(3).
 */
const char *date_parse_timestamp(const char *text, struct tm *base_tm,
                                 time_t * t)
{
    const char *parse_end = date_parse_date(text, base_tm, t);

    if (parse_end != NULL)
    {
        const char *time_end;

        if (*parse_end == 'T')
        {
            parse_end += 1;
        }
        while (*parse_end == ' ')
        {
            parse_end += 1;
        }
        if ((time_end = date_parse_time(parse_end, base_tm, t)) != NULL)
        {
            return time_end;           /* success: date+time */
        }
    }
    else if ((parse_end = strptime(text, "@%s", base_tm)) != NULL)
    {
        if (parse_end == text)
        {                              /* huh!? we parsed nothing */
            parse_end = NULL;          /* REVISIT: is this a macos quirk? */
        }
        else
        {
            base_tm->tm_isdst = -1;    /* force recalculation of dst */
            *t = mktime(base_tm);
        }

    }
    return parse_end;                  /* success/fail */
}

/*
 * fmt_time() --Format a time_t value using strftime().
 *
 * Parameters:
 * str  --returns the formatted string
 * size --specifies the size of str
 * fmt  --specifies a strftime(3) format
 * t    --specifies a timestamp (value 0 => current time)
 *
 * Returns: (char *)
 * Success: the end of the string; Failure: str.
 *
 * Remarks:
 * strftime()'s wonderful, but it needs a tm struct to do its
 * magic.  This routine creates one on-the-fly.
 */
char *fmt_time(char *str, size_t size, const char *fmt, time_t t)
{
    struct tm tm;

    if (t == 0)
    {
        time(&t);                      /* default time: now! */
    }
    localtime_r(&t, &tm);

    return str + strftime(str, size, fmt, &tm);
}
