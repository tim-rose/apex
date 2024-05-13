/*
 * TEST-LOG.C --Unit tests for logging.
 *
 * Contents:
 * default_log_state[] --Fixture for logging state at start of test.
 * test_log_state[]    --Captures the current state of the logger.
 * mock_log_output()   --A mocked log output routine, for testing.
 * main()              --Run some unit tests.
 *
 */
#include <string.h>
#include <time.h>

#include <apex/test.h>
#include <apex/log.h>

#define LOG_TEXT_MAX 60
static int mock_log_output(const LogConfig * UNUSED(config),
                           const LogContext * caller,
                           int sys_errno, size_t priority,
                           const char *fmt, va_list args);

typedef struct LogState
{
    int sys_errno;
    int priority;
    char text[LOG_TEXT_MAX + 1];
} LogState, *LogStatePtr;

/*
 * default_log_state[] --Fixture for logging state at start of test.
 */
static LogState default_log_state = {
    .sys_errno = -1,
    .priority = -1,
    .text = {0}
};

static LogState log_state;

/*
 * test_log_state[] --Captures the current state of the logger.
 */
static LogConfig test_log_state = {
    .threshold_priority = LOG_NOTICE,  /* default values */
    .identity = "id",
    .facility = LOG_USER,
    .output = mock_log_output
};

/*
 * mock_log_output() --A mocked log output routine, for testing.
 *
 * Remarks:
 * This routine just stashes the formatted message and the rest of the
 * arg.s so that a test can inspect it later.
 */
static int mock_log_output(const LogConfig * UNUSED(config),
                           const LogContext * caller,
                           int sys_errno, size_t priority,
                           const char *fmt, va_list args)
{
    log_state.text[0] = '\0';
    log_state.sys_errno = sys_errno;
    log_state.priority = priority;

    return log_vsprintf(caller, log_state.text, LOG_TEXT_MAX + 1,
                        sys_errno, priority, fmt, args);
}

/*
 * main() --Run some unit tests.
 */
int main(void)
{
    log_init("log-test");
    log_config(&test_log_state);

    plan_tests(8);
    log_state = default_log_state;
    notice("test message");
    string_eq(log_state.text, "notice: test message",
              "simple message is prefixed with priority");
    number_eq(log_state.priority, LOG_NOTICE, "%d",
              "message is logged at eponymous priority");
    number_eq(log_state.sys_errno, 0, "%d",
              "system error is not printed by default");

    log_state = default_log_state;
    debug("test message");
    string_eq(log_state.text, "", "low priority messages are not printed");

    log_state = default_log_state;
    notice("%s test message", "dynamic");
    string_eq(log_state.text,
              "notice: dynamic test message",
              "log messages support printf() arg.s");

    log_state = default_log_state;
    notice("1234567890" "1234567890" "1234567890" "1234567890" "1234567890"
           "1234567890" "1234567890" "1234567890" "1234567890" "1234567890");
    string_eq(log_state.text,
              "notice: "
              "1234567890" "1234567890" "1234567890" "1234567890" "1234567890"
              "12",
              "log messages are truncated to %d characters", LOG_TEXT_MAX);

    errno = 1;                         /* Operation not permitted */
    log_state = default_log_state;
    log_sys(LOG_NOTICE, "test message");
    number_eq(log_state.priority, LOG_NOTICE, "%d",
              "log_sys() uses provided priority");
    string_eq(log_state.text,
              "notice: test message: Operation not permitted",
              "log_sys() applies a priority prefix, and appends the system error");
    return exit_status();
}
