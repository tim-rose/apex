/*
 * STATELY.H --A simple hierarchic state machine.
 *
 * Contents:
 * StatelyActionProc() --The stately action procedure.
 * StatelyState{}      --The stately machine state.
 * Stately{}           --The stately state machine object.
 */
#ifndef STATELY_H
#define STATELY_H

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */

/*
 * StatelyActionProc() --The stately action procedure.
 */
typedef int (*StatelyActionProc)(int   state_id,
                                 int   event_id,
                                 void* event_ctx,
                                 void* ctx);

/*
 * StatelyState{} --The stately machine state.
 *
 * Remarks:
 * Defines the action routines for this state as an array indexed by
 * the event id.
 */
typedef struct StatelyState
{
    int               init_state;       /* compound state's initial sub-state  */
    int               parent_id;        /* ID of parent state in table */
    StatelyActionProc enter, exit, *action;
} StatelyState;

/*
 * Stately{} --The stately state machine object.
 *
 * Remarks:
 * Defines an array of StatelyStates indexed by the state id.
 */
typedef struct Stately
{
    int           state_id;             /* current state */
    void*         context;              /* extended/external state */
    StatelyState* state;                /* state definitions */
} Stately;

Stately* stately_init(Stately* stc, void* context);
int      stately_event(Stately* stc, int event_id, void* event_ctx);
void     stately_reset(Stately* stc);
#ifdef __cplusplus
}
#endif /* C++ */
#endif /* STATELY_H */
