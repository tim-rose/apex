/*
 * TURSTILE.C --Test stately with a simple turnstile statechart.
 *
 * Remarks:
 * This test suite implements a simple turnstile statechart, and
 * tests the outcomes of various transitions.
 * The action routines are setup to record their activity in a
 * global buffer, which is used as part of the test results.
 */
/* LCOV_EXCL_START */
#include <apex.h>
#include <apex/log.h>
#include <apex/stately.h>
#include <apex/tap.h>
#include <apex/test.h>

static char actions[100];

typedef enum TurnstileEvent
{
    coin,
    turn,
    refund,
    n_events /* sentinel/size */
} TurnstileEvent;

typedef enum TurnstileState
{
    init,
    locked,
    unlocked,
    n_states /* sentinel/size */
} TurnstileState;

static int ignore_event(int state_id, int event_id, void* event_ctx, void* ctx);
static int coin_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int turn_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int refund_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int lock_turnstile(int state_id, int event_id, void* event_ctx, void* ctx);
static int unlock_turnstile(int state_id, int event_id, void* event_ctx, void* ctx);

static StatelyActionProc turnstile_actions[n_states][n_events] = {
    [init] =
        {
            ignore_event,
            ignore_event,
            ignore_event,
        },
    [locked] =
        {
            [coin] = coin_action, /* and go to unlocked state */
        },
    [unlocked] =
        {
            [turn]   = turn_action, /* and go to locked state */
            [refund] = refund_action,
        },
};

static StatelyState turnstile_states[n_states] = {
    [init] =
        {
            .init_state = locked,
            .action     = turnstile_actions[init],
        },
    [locked] =
        {
            .enter  = lock_turnstile,
            .action = turnstile_actions[locked],
        },
    [unlocked] =
        {
            .enter  = unlock_turnstile,
            .action = turnstile_actions[unlocked],
        },
};

static Stately turnstile = {.state = turnstile_states};

static void test_turnstile(void);

int main(int UNUSED(argc), char* UNUSED(argv[]))
{
    plan_tests(10);

    test_turnstile();
    return exit_status();
}

static void test_turnstile(void)
{
    actions[0] = '\0';
    stately_init(&turnstile, NULL);
    string_eq(actions, "1/-1: locked! ", "on init: lock() is called");
    number_eq(turnstile.state_id, 1, "%d", "on init: transition to state 1");

    actions[0] = '\0';
    stately_event(&turnstile, turn, NULL);
    string_eq(actions, "1/1: ignored ", "on turn: ignored");
    number_eq(turnstile.state_id, 1, "%d", "on turn: remain in state 1");

    actions[0] = '\0';
    stately_event(&turnstile, coin, NULL);
    string_eq(actions, "1/0: coin 2/0: unlocked! ", "on coin: unlock() is called");
    number_eq(turnstile.state_id, 2, "%d", "on coin: transition to state 2");

    actions[0] = '\0';
    stately_event(&turnstile, turn, NULL);
    string_eq(actions, "2/1: turn 1/1: locked! ", "on turn: lock() is called");
    number_eq(turnstile.state_id, 1, "%d", "on turn: transition to state 1");

    actions[0] = '\0';
    stately_reset(&turnstile);
    string_eq(actions, "1/-1: locked! ", "on reset: lock() is called");
    number_eq(turnstile.state_id, 1, "%d", "on reset: transition to state 1");
}

static void log_event(int state_id, int event_id, const char* msg)
{
    char text[100];

    sprintf(text, "%d/%d: %s ", state_id, event_id, msg);
    strcat(actions, text);
}

static int ignore_event(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "ignored");
    return state_id;
}

static int coin_action(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "coin");
    return unlocked;
}

static int turn_action(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "turn");
    return locked;
}

static int refund_action(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "refund");
    return locked;
}

static int lock_turnstile(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "locked!");
    return state_id;
}

static int unlock_turnstile(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "unlocked!");
    return state_id;
}
/* LCOV_EXCL_STOP */
