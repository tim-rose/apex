/*
 * CONVERT.C --Unit tests for unit(!) conversions.
 *
 */
#include <string.h>

#include <xtd/tap.h>
#include <xtd.h>
#include <xtd/convert.h>

int main(void)
{
    double value;

    plan_tests(17);

    ok(str_convert("1m", &value, convert_length), "convert: 1m");
    ok(FEQUAL(1.0, value, 0.00001), "convert: 1m (value)");

    ok(str_convert("1ft", &value, convert_length), "convert: 1ft");
    ok(FEQUAL(1.0 * .3048, value, 0.00001), "convert: 1ft (value)");

    ok(str_convert("1kph", &value, convert_velocity), "convert: 1kph");
    ok(FEQUAL(1 / 3.6, value, 0.00001), "convert: 1kph (value)");

    ok(str_convert("1ms", &value, convert_duration), "convert: 1ms");
    ok(FEQUAL(0.001, value, 0.00001), "convert: 1ms (value)");

    ok(str_convert("32F", &value, convert_temperature), "convert: 32F");
    ok(FEQUAL(0.0, value, 0.00001), "convert: 32F (value)");
    str_convert("100F", &value, convert_temperature);
    ok(FEQUAL(37.7777, value, 0.00001), "convert: 100F (value)");
    ok(str_convert("-40F", &value, convert_temperature), "convert: -40F");
    ok(FEQUAL(-40.0, value, 0.00001), "convert: -40F (value)");
    ok(str_convert("273K", &value, convert_temperature), "convert: 273K");
    ok(FEQUAL(0.15, value, 0.00001), "convert: 273K (value)");

    ok(!str_convert("1", &value, convert_duration),
       "convert: no unit specified");
    ok(!str_convert("1bogomips", &value, convert_duration),
       "convert: unknown unit specified");

    return exit_status();
}
