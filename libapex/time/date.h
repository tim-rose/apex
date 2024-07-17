/*
 * DATE.H --Definitions for date/timestamp functions.
 *
 */
#ifndef DATE_H
#define DATE_H

#define __TM_GMTOFF tm_gmtoff          /* REVISIT: enable tm_gmtoff on cygwin */
#include <time.h>
#include <apex.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
#ifdef __WINNT__
    extern char* strptime(const char *buf, const char *fmt, struct tm *tm);
#endif /* __WINNT__ */
    enum date_consts
    {
        TIME_T_MIN = 0,                /* REVIST: wrong minimum! -1 */
        TIME_T_MAX = 0x7fffffff        /* bit of a hack, but hey... */
    };

    extern struct tm null_tm;
    extern const char date_syslog_timestamp[];
    extern const char date_ISO8601_timestamp[];
    extern const char date_ISO8601_date[];
    extern const char date_ISO8601_time[];

    const char *date_parse_fmt(const char *text, struct tm *base_tm,
                               const char *fmt[], size_t n_fmt,
                               const char **match_fmt);

    const char *date_parse_date(const char *text, struct tm *base_tm,
                                time_t * t);
    const char *date_parse_time(const char *text, struct tm *base_tm,
                                time_t * t);
    const char *date_parse_timestamp(const char *text, struct tm *base_tm,
                                     time_t * t);
    char *fmt_time(char *str, size_t size, const char *fmt, time_t t)
        STRFTIME_ATTRIBUTE(3);
    int adjust_ut(time_t * t, int delta, char *unit);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* DATE_H */
