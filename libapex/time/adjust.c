/*
 * DATE.C --Date and time manipulation and parsing functions.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * adjust_ut() --Adjust a timestamp by a unit+delta.
 *
 * Remarks:
 * This file contains code for adjusting timestamps.
 */

#include <apex.h>
#include <apex/date.h>
#include <apex/estring.h>

#ifdef __WINNT__
#define localtime_r(timep_, result_) localtime(timep_)
#endif /* __WINNT__ */

/*
 * adjust_ut() --Adjust a timestamp by a unit+delta.
 *
 * Parameters:
 * t        --(a pointer to) the time_t/epoch to be adjusted
 * delta, unit  --the quantity and unit size of the delta to apply
 *
 * Returns: (int)
 * Success: 1;  Failure: 0.
 */
int adjust_ut(time_t * t, int delta, char *unit)
{
    int *field = NULL;
    struct tm t_tm;

    localtime_r(t, &t_tm);
    switch (*unit)
    {
    case 's':
    case 'S':
        if (vstrcasematch(unit, "s", "second", "seconds", NULLPTR) >= 0)
        {
            field = &t_tm.tm_sec;
        }
        break;
    case 'M':
        if (vstrcasematch(unit, "m", "month", "months", NULLPTR) >= 0)
        {
            goto unit_month;
        }
      unit_minute:
        if (vstrcasematch(unit, "m", "minute", "minutes", NULLPTR) >= 0)
        {
            field = &t_tm.tm_min;
        }
        break;
    case 'h':
    case 'H':
        if (vstrcasematch(unit, "h", "hour", "hours", NULLPTR) >= 0)
        {
            field = &t_tm.tm_hour;
        }
        break;
    case 'd':
        if (vstrcasematch(unit, "d", "day", "days", NULLPTR) >= 0)
        {
            field = &t_tm.tm_mday;
        }
        break;
    case 'w':
        if (vstrcasematch(unit, "w", "week", "weeks", NULLPTR) >= 0)
        {
            field = &t_tm.tm_mday;
        }
        delta *= 7;
        break;
    case 'm':
        if (vstrcasematch(unit, "m", "minute", "minutes", NULLPTR) >= 0)
        {
            goto unit_minute;
        }
      unit_month:
        if (vstrcasematch(unit, "m", "month", "months", NULLPTR) >= 0)
        {
            field = &t_tm.tm_mon;
        }
        break;
    case 'y':
    case 'Y':
        if (vstrcasematch(unit, "y", "year", "years", NULLPTR) >= 0)
        {
            field = &t_tm.tm_year;
        }
        break;
    default:
        return 0;                      /* error: bad delta unit */
        break;                         /* NOTREACHED */
    }
    if (field != NULLPTR)
    {
        *field += delta;
        t_tm.tm_isdst = -1;            /* dst may be different now */
        *t = mktime(&t_tm);            /* "normalise" the time, set timezone */
        return 1;
    }
    return 0;
}
