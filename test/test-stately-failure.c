/*
 * FAILURE.C --Test stately boundary and failure conditions.
 *
 * Remarks:
 * This test suite extends the simple turnstile statechart with some
 * sub-states and some action/transition errors to simulate the
 * various boundary conditions and failure checks performed by the
 * statechart.
 */
/* LCOV_EXCL_START */
#include <apex.h>
#include <apex/log.h>
#include <apex/tap.h>
#include <apex/stately.h>
#include <apex/test.h>

static char actions[100];

typedef enum TurnstileEvent
{
    coin,
    turn,
    refund,
    timeout,
    n_events /* sentinel/size */
} TurnstileEvent;

typedef enum TurnstileState
{
    init,
    locked,
    sub_locked,
    unlocked,
    sub_unlocked,
    n_states /* sentinel/size */
} TurnstileState;

static int coin_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int turn_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int refund_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int timeout_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int error_action(int state_id, int event_id, void* event_ctx, void* ctx);
static int lock_turnstile(int state_id, int event_id, void* event_ctx, void* ctx);
static int unlock_turnstile(int state_id, int event_id, void* event_ctx, void* ctx);
static int enter_sub_lock(int state_id, int event_id, void* event_ctx, void* ctx);
static int enter_sub_unlock(int state_id, int event_id, void* event_ctx, void* ctx);
static int exit_lock(int state_id, int event_id, void* event_ctx, void* ctx);

/* clang-format off */
static StatelyActionProc turnstile_actions[n_states][n_events] = {
    [init] = { 0 },                /* no default actions */
    [locked] = {
        [coin] = coin_action,       /* and go to unlocked state */
        [refund] = error_action},       /* -> invalid state */
    [unlocked] = {
        [turn] = turn_action, /* and go to locked state */
        [refund] = refund_action,
        [timeout] = timeout_action},
    [sub_unlocked] = { 
        [timeout] = timeout_action},
};

static StatelyState turnstile_states[n_states] = {
    [init]     = {
        .init_state = locked, 
        .action = turnstile_actions[init]},
    [locked]   = {
        .init_state = sub_locked, 
        .enter = lock_turnstile, 
        .exit = exit_lock, 
        .action = turnstile_actions[locked]},
    [sub_locked] = {
        .parent_id = locked, 
        .enter = enter_sub_lock,
        .action = NULL},         /* force stately's fallback action handling */
    [unlocked] = {
        .init_state = sub_unlocked, 
        .enter = unlock_turnstile, 
        .action = turnstile_actions[unlocked]},
    [sub_unlocked] = {
        .parent_id = unlocked, 
        .enter = enter_sub_unlock,
        .action = turnstile_actions[sub_unlocked]},
};
/* clang-format on */

static Stately turnstile = {.state = turnstile_states};

static void test_failure(void);

int main(int UNUSED(argc), char* UNUSED(argv[]))
{
    plan_tests(16);

    test_failure();
    return exit_status();
}

static void test_failure(void)
{
    stately_init(&turnstile, NULL);

    actions[0] = '\0';
    stately_event(&turnstile, turn, NULL);
    string_eq(actions, "", "on turn: nothing happens");
    number_eq(turnstile.state_id, sub_locked, "%d", "on turn: remain in state %d", sub_locked);

    actions[0] = '\0';
    stately_event(&turnstile, coin, NULL);
    string_eq(
        actions, "2/0: coin 1/0: exit locked 3/0: unlocked! 4/0: sub-unlocked! ", "on coin: sub-unlock is called");
    number_eq(turnstile.state_id, sub_unlocked, "%d", "on coin: transition to state %d", sub_unlocked);

    actions[0] = '\0';
    stately_event(&turnstile, -1, NULL);
    string_eq(actions, "", "on invalid event: nothing happens");
    number_eq(turnstile.state_id, sub_unlocked, "%d", "on invalid event: no state change");

    actions[0] = '\0';
    stately_event(&turnstile, timeout, NULL);
    string_eq(actions, "4/3: timeout ", "on timeout: timeout is called");
    number_eq(turnstile.state_id, unlocked, "%d", "on timeout: transition to state %d", unlocked);

    actions[0] = '\0';
    stately_event(&turnstile, timeout, NULL); /* toggle to parent state */
    string_eq(actions, "3/3: timeout 4/3: sub-unlocked! ", "on timeout (again): timeout is called");
    number_eq(turnstile.state_id, sub_unlocked, "%d", "on timeout (again): transition to state %d", sub_unlocked);
    stately_event(&turnstile, timeout, NULL); /* toggle back to parent state */

    actions[0] = '\0';
    stately_event(&turnstile, turn, NULL);
    string_eq(actions, "3/1: turn 1/1: locked! 2/1: sub-locked! ", "on turn: sub-lock() is called");
    number_eq(turnstile.state_id, sub_locked, "%d", "on turn: transition to state %d", sub_locked);

    actions[0] = '\0';
    stately_event(&turnstile, refund, NULL);
    string_eq(actions, "2/2: refund-error ", "on locked/refund: refund-error is called");
    number_eq(turnstile.state_id, -1, "%d", "on locked/refund: transition to error state");

    actions[0] = '\0';
    stately_event(&turnstile, coin, NULL);
    string_eq(actions, "", "on invalid/coin: nothing happens");
    number_eq(turnstile.state_id, -1, "%d", "on invalid/coin: no state change");
}

static void log_event(int state_id, int event_id, const char* msg)
{
    char text[100];

    sprintf(text, "%d/%d: %s ", state_id, event_id, msg);
    strcat(actions, text);
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

static int error_action(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "refund-error");
    return sub_unlocked;
}

static int timeout_action(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "timeout");
    return state_id == unlocked ? sub_unlocked : unlocked;
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

static int enter_sub_lock(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "sub-locked!");
    return state_id;
}

static int enter_sub_unlock(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "sub-unlocked!");
    return state_id;
}

static int exit_lock(int state_id, int event_id, void* UNUSED(event_ctx), void* UNUSED(ctx))
{
    log_event(state_id, event_id, "exit locked");
    return state_id; /* ignored */
}
/* LCOV_EXCL_STOP */
