/*
 * STATELY.C --A simple restricted Harel statechart.
 *
 * Contents:
 * stately_init()     --Initialise a Stately statechart.
 * stately_reset()    --Reset a Stately statechart.
 * stately_event()    --Process an event.
 * stately_enter()    --Enter a new state, follow its init_state.
 * stately_exit()     --Exit substates of a target state.
 * stately_ancestor() --Return the shared ancestor of two states.
 * stately_action()   --Find the action for this event.
 *
 * Remarks:
 * This module implements a really simple statechart, i.e. an hierarchic
 * state machine.  The hierarchic behaviour boils down to:
 * * states can be "substates" of another "parent" state
 * * states have optional "entry" and "exit" actions
 * * substates can inherit behaviour (action functions) from their parent.
 *
 * It does not implement:
 * * transition actions
 * * direct transition to substates (but maybe one day)
 * * history
 * * deferred actions
 * * regions.
 *
 * Also it does not implement timeouts or event queues; if you want those
 * things, they are easy to incorporate from pre-existing frameworks,
 * please use them.
 */
#include <xtd.h>
#include <xtd/log.h>
#include <xtd/stately.h>

static int  stately_enter(Stately* stc,
                          int      state_id,
                          int      event_id,
                          void*    event_ctx);
static void stately_exit(Stately* stc,
                         int      target_id,
                         int      event_id,
                         void*    event_ctx);

static StatelyActionProc stately_action(Stately* stc, int event_id);
static int stately_ancestor(Stately* stc, int target_id, int state_id);

/*
 * stately_init() --Initialise a Stately statechart.
 *
 * Parameters:
 * stc  --specifies the statechart
 *
 * Returns: (Stately *)
 * stc
 */
Stately* stately_init(Stately* stc, void* context)
{
    stc->context  = context;
    stc->state_id = stately_enter(stc, 0, -1, NULL);
    return stc;
}

/*
 * stately_reset() --Reset a Stately statechart.
 *
 * Parameters:
 * stc  --specifies the statechart
 *
 * Remarks:
 * This routine exits any current substates and re-enters state 0.
 */
void stately_reset(Stately* stc)
{
    stately_exit(stc, 0, -1, NULL);
    stc->state_id = stately_enter(stc, 0, -1, NULL);
}

/*
 * stately_event() --Process an event.
 *
 * Parameters:
 * stc  --specifies the statechart
 * event_id     --specifies the event
 * event_ctx    --specifies additional event context/values
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This routine applies an action associated with the event in
 * the current state of the statechart.  It is the action's
 * responsibility to specify the next state, and the transition
 * is done here.
 *
 * If a transition is made, the stc leaves all the substates of
 * the current state to a common ancestor of the new state.
 * Effectively this means that a transition can traverse outwards
 * from substates to some eventual sibling with a common ancestor.
 *
 * REVISIT: implement direct transitions to substates, LCA.
 */
int stately_event(Stately* stc, int event_id, void* event_ctx)
{
    int state_id     = stc->state_id;
    int new_state_id = state_id;

    debug("%s(): state_id=%d, event_id=%d", __func__, state_id, event_id);
    if (state_id < 0)
    {
        return 0; /* failure: invalid state */
    }
    StatelyActionProc action = stately_action(stc, event_id);

    if (action)
    {
        debug("%s(): action=0x%p", __func__, (void*)action);
        new_state_id = action(state_id, event_id, event_ctx, stc->context);
    }
    if (new_state_id != state_id)
    { /* transition */
        StatelyState const* state       = &stc->state[state_id];
        StatelyState const* new_state   = &stc->state[new_state_id];
        int                 ancestor_id = 0;

        debug(
            "%s(): current(state=%d, parent=%d), new(state=%d, parent=%d), "
            "ancestor_id=%d",
            __func__,
            state_id,
            state->parent_id,
            new_state_id,
            new_state->parent_id,
            ancestor_id);

        if (new_state->parent_id == state_id)
        {
            debug("%s(): enter child state %d", __func__, new_state_id);
            stc->state_id =
                stately_enter(stc, new_state_id, event_id, event_ctx);
            return 1; /* success: new state is a direct substate */
        }

        ancestor_id = stately_ancestor(stc, new_state_id, state_id);
        if (ancestor_id == new_state_id)
        {
            debug("%s(): exit to state %d", __func__, ancestor_id);
            stately_exit(stc, ancestor_id, event_id, event_ctx);
            stc->state_id = ancestor_id;
            return 1; /* success: old state is a substate */
        }

        ancestor_id = stately_ancestor(stc, new_state->parent_id, state_id);
        if (ancestor_id >= 0)
        {
            debug("%s(): exit to state %d, enter state %d",
                  __func__,
                  ancestor_id,
                  new_state_id);
            stately_exit(stc, ancestor_id, event_id, event_ctx);
            stc->state_id =
                stately_enter(stc, new_state_id, event_id, event_ctx);
            return 1; /* success: states with common parents */
        }
        else
        {
            stc->state_id = -1; /* invalid state */
            return 0;           /* failure: no common ancestor!? */
        }
    }
    return 1; /* success: event was actioned */
}

/*
 * stately_enter() --Enter a new state, follow its init_state.
 *
 * Parameters:
 * stc  --specifies the state machine
 * state_id     --specifies the state
 * event_id     --specifies the event
 * event_ctx    --specifies additional event context/values
 *
 * Returns: (int)
 * The next state.
 */
static int stately_enter(Stately* stc,
                         int      state_id,
                         int      event_id,
                         void*    event_ctx)
{
    for (;;) /* REVISIT: infinite loop, limit to n_states? */
    {
        StatelyState* state = &stc->state[state_id];

        if (state->enter)
        {
            debug("%s(): enter=0x%p", __func__, (void*)state->enter);
            state->enter(state_id, event_id, event_ctx, stc->context);
        }
        if (state->init_state != 0)
        {
            debug(
                "%s(): state=%d -> %d", __func__, state_id, state->init_state);
            state_id = state->init_state;
        }
        else
        {
            break;
        }
    }
    return state_id;
}

/*
 * stately_exit() --Exit substates of a target state.
 *
 * Parameters:
 * stc  --specifies the state machine
 * target_id     --specifies the target/parent state
 * event_id     --specifies the event
 * event_ctx    --specifies additional event context/values
 */
static void stately_exit(Stately* stc,
                         int      target_id,
                         int      event_id,
                         void*    event_ctx)
{
    StatelyState* state;

    debug("%s(): target_id=%d", __func__, target_id);
    for (int state_id = stc->state_id; state_id != target_id;
         state_id     = state->parent_id)
    {
        state = &stc->state[state_id];

        debug("    %s(): state_id=%d", __func__, state_id);
        if (state->exit)
        {
            debug("    %s(): exit=0x%p", __func__, (void*)state->exit);
            state->exit(state_id, event_id, event_ctx, stc->context);
        }
    }
}

/*
 * stately_ancestor() --Return the shared ancestor of two states.
 *
 * Parameters:
 * stc  --specifies the state machine
 * target_id --specifies the expected ancestor state
 * state_id	--specifies the child substate
 *
 * Returns: (int)
 * Success: the ancestor state; Failure: -1.
 *
 * Remarks:
 * This just confirms (or confutes) that the posited ancestor
 * is truly an ancestor of the specified state.
 *
 * REVISIT: Least Common Ancestor (LCA) for direct substate transitions.
 */
static int stately_ancestor(Stately* stc, int target_id, int state_id)
{
    while (state_id != target_id)
    {
        if (state_id == 0)
        {
            return -1; /* failure: state 0 has no parents */
        }
        state_id = stc->state[state_id].parent_id;
    }
    return state_id; /* success: return common ancestor */
}

/*
 * stately_action() --Find the action for this event.
 *
 * Parameters:
 * stc  --specifies the statechart
 * event_id     --specifies the event
 *
 * Returns: (StatelyActionProc)
 * Success: a pointer to the action function; Failure: NULL.
 */
static StatelyActionProc stately_action(Stately* stc, int event_id)
{
    StatelyState* state; /* initialised in loop */

    if (event_id < 0)
    {
        return NULL; /* failure: no such event */
    }
    for (state = &stc->state[stc->state_id]; state != &stc->state[0];
         state = &stc->state[state->parent_id])
    {
        if (state->action != NULL && state->action[event_id] != NULL)
        {
            break;
        }
    }
    return state->action[event_id]; /* success: possibly NULL */
}
