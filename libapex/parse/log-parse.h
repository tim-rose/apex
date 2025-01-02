/*
 * LOG-PARSE.H --Routines for parsing syslog records.
 */
#ifndef APEX_LOG_PARSE_H
#define APEX_LOG_PARSE_H

#include <apex.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include SYSLOG
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    enum log_parse_consts
    {
        SYSLOG_LINE_MAX = LINE_MAX,    /* maximum record size */
    };

    /*
     * LogRecord --A parsed syslog record
     */
    typedef struct LogRecord_t
    {
        time_t timestamp;
        char *host;
        char *tag;
        pid_t pid;                     /* parsed pid, or -1 */
        int facility;                  /* parsed facility, or -1 */
        int priority;                  /* parsed priority, or -1 */
        char *message;
        char text[SYSLOG_LINE_MAX];    /* original text, munged */
    } LogRecord, *LogRecordPtr;

    LogRecordPtr log_parse_r(LogRecordPtr log_record,
                             const char *str, struct tm *tm_base);
    LogRecordPtr log_parse(const char *str, struct tm *tm_base);
    LogRecordPtr log_fgets(LogRecordPtr log_record, FILE * fp,
                           struct tm *tm_base);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_LOG_PARSE_H */
