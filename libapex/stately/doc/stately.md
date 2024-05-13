# Stately: A Simple Statechart

Stately is an implementation of a few of the ideas in Harel's original
statechart.

It implements the core hierarchic behaviour:

* states can be "substates" of another "parent" state
* states have optional "entry" and "exit" actions
* substates can inherit behaviour (action functions) from their parent.

It does not implement:

* transition actions
* direct transition to substates (but maybe one day)
* history
* deferred actions
* regions.

## External State

Although the state machine can be in only one defined state at a time,
most practical examples of state machines have some extended notion of
state, e.g. to refine transition logic, accumulate results.  Stately
provides a context field to store this extra information, which is
available as a parameter the event action procedure.

## Event Data

Stately processes events by type only, but events may have additional
value(s) associated woth them.  E.g. a "keypress" event may have the
key value and shift-keys state.  This extended state can be supplied
to the event action procedure.

## Action Procedures

Stately dispatches events to action procedures defined for the current
state.  If no action is defined for that state, the parent's action is
used, recursively.

The action procedure decides the next state by return value.

### Transition Procedures

When a state is entered or exited, state-specific entry and exit
routines will be called if they are defined.  Exit routines  are called in
sequence up the state hierarchy up to the least common parent of the
new state, and then the new state's enter routine is called.

## Turnstile: an Example

This example defines a simple turnstile state machine; It transitions
to the lock, unlock states, based on events such as coin payment and
rotating the turnstile.

![turnstile statechart](turnstile.png)

### State, Event IDs

Let's define the states and events of the turnstile as C enums.

```c {.numberLines}
typedef enum  { init, locked, unlocked, n_states } TurnstileState;
typedef enum  { coin, turn refund, n_events } TurnstileEvent;
```

### Actions

The actions taken by the statechart for each event and state may be arranged
as a table. Note that only the events where an action is taken need be
specified; the remaining events will perform actions defined by the parent in
the state hierarchy, if any.  This works because the fields
unspecified in the definition will be set to 0/NULL.

In this example, the actions not specified in
the `locked` and `unlocked` states will fallback to the `init` handling, which
calls `ignore_event()`.

```c {.numberLines}
static StatelyActionProc turnstile_actions[n_states][n_events] = {
    [init] = { ignore_event, ignore_event, ignore_event },
    [locked] = {
        [coin] = coin_action }, /* and go to unlocked state */
    [unlocked] = {
        [turn]   = turn_action, /* and go to locked state */
        [refund] = refund_action },
};
```

The actions are laid out in the `n_states`x`n_events` table for
convenience.  The action lists and other controlling information is specified
as a `StatelyState` array.

### States

An `n_states` array of state specifications define the overall
behaviour of the state machine.

```c {.numberLines}
static StatelyState turnstile_states[n_states] = {
    [init] = {
        .init_state = locked,
        .action = turnstile_actions[init] },
    [locked] = {
        .enter = lock_turnstile,
        .action = turnstile_actions[locked] },
    [unlocked] = {
        .enter = unlock_turnstile,
        .action = turnstile_actions[unlocked] }
};
```

Notes

* The initial state of the turnstile is "locked", this is
  achieved by setting the `.init_state` field to `locked`.
* The `.action` fields each reference a row in the `turnstile_actions`.
* The action of locking/unlocking the turnstile is delegated to the
  state entry routines.

### State Machine

The full state machine manages some internal state and context, which
in this simple case is not used.  Note that the initial state is
unspecified, so it defaults to 0.

```c {.numberLines}
static Stately turnstile = {.state = turnstile_states};
```
