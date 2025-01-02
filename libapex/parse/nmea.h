/*
 * NMEA.H --Parsing/Processing routines for NMEA messages.
 *
 * Remarks:
 * NMEA is a simple protocol that is used by nautical devices, and has
 * come into common use via GPS units.  It is defined by a simple
 * "sentence" structure; each sentence:
 * * is at most 80 characters long,
 * * is entirely printable ASCII
 * * can have an (optional) checksum trailer.
 *
 * The overall structure of a sentence is:
 * $        --start of line character (a literal ASCII '$')
 * TT       --two character "talker" ID
 * MMM      --three character message ID
 * C,S,V,...    --comma-separated values, as determined by the message
 * *xx      --an optional checksum, comprised of a literal '*' followed
 *            by two hexadecimal digits representing the XOR of ALL
 *            the data between the initial '$' and the '*'
 * <cr><lf> --typical DOS-ish end-of-line.
 *
 */
#include <stdio.h>
#ifndef APEX_NMEA_H
#define APEX_NMEA_H
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    enum nmea_consts
    {
        NMEA_LINE_MAX = 80,            /* maximum record size */
        NMEA_OK = 0,
        NMEA_EOF = EOF,                /* imported from stdio */
        NMEA_ERR = -2,                 /* miscellaneous failure */
    };

    typedef struct Nmea_t
    {
        char id[3];                    /* talker ID */
        char msg[4];                   /* message ID */
        size_t n_values;               /* No. used slots in value[] */
        char *value[NMEA_LINE_MAX];    /* CSV values */
        char text[NMEA_LINE_MAX + 3];
    } Nmea, *NmeaPtr;

    unsigned int nmea_checksum(const char *str, const char *end);
    int nmea_parse(NmeaPtr nmea_buf, const char *str);
    int nmea_fget(NmeaPtr nmea_buf, FILE * fp);
    char *nmea_fmt(NmeaPtr nmea_buf, char *str);
    int nmea_fputs(NmeaPtr nmea_buf, FILE * fp);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_NMEA_H */
