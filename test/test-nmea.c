/*
 * NMEA.C --Unit tests for the "NMEA" functions.
 *
 * Remarks:
 *
 */
#include <stdio.h>
#include <string.h>

#include <tap.h>
#include <estring.h>
#include <nmea.h>

int main(void)
{
    plan_tests(13);
    Nmea nmea;
    char text[NMEA_LINE_MAX + 3];

    ok(nmea_parse(&nmea, "$IxMxx*7C\r\n") == NMEA_OK, "trivial message");
    ok(nmea_parse(&nmea, "$IxMxx\r\n") == NMEA_OK, "checksum can be omitted");
    ok(nmea_parse(&nmea, "$IxMxx\n") == NMEA_OK, "<cr> can be omitted");
    ok(strcmp(nmea.id, "Ix") == 0, "parse ID");
    ok(strcmp(nmea.msg, "Mxx") == 0, "parse MSG");
    ok(nmea.n_values == 1, "empty payload count");

    /*
     * simple payload
     */
    ok(nmea_parse(&nmea, "$IxMxxpayload\n") == NMEA_OK, "get simple payload");
    ok(nmea.n_values == 1, "simple payload count");
    if (!ok(strequiv(nmea.value[0], "payload") == 0, "simple payload value"))
    {
        diag((char *) "expected \"%s\", got \"%s\"", "payload",
             nmea.value[0]);
    }

    /*
     * compound payload
     */
    ok(nmea_parse(&nmea, "$IxMxxthe,quick,brown,fox\n") == NMEA_OK,
       "get compound payload");
    ok(nmea.n_values == 4, "compound payload count");
    ok(strequiv(nmea.value[0], "the") == 0
       && strequiv(nmea.value[1], "quick") == 0
       && strequiv(nmea.value[2], "brown") == 0
       && strequiv(nmea.value[3], "fox") == 0, "compound payload values");

    nmea_fmt(&nmea, text);
    if (!ok(strequiv(text, "$IxMxxthe,quick,brown,fox*5B\r\n") == 0,
            "format compound record"))
    {
        diag((char *) "format failed: got \"%s\"", text);
    }

    return exit_status();
}
