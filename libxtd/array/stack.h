/*
 * STACK.H --A simple stack container, with caller provided item storage.
 */
#ifndef STACK_H
#define STACK_H

#include <xtd.h>
#include <array.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct Stack_t
    {
        ArrayContainer container;
        size_t position;
    } Stack, *StackPtr;

    StackPtr stack_alloc(void);
    StackPtr stack_init(StackPtr stack, size_t n_items,
                        size_t item_size, void *items);

    int stack_push(StackPtr stack, const void *item);
    int stack_pop(StackPtr stack, void *item);
    void *stack_peek(StackPtr stack);

#define new_stack(items) init_stack(stack_alloc(), items)
#define init_stack(stack, items) stack_init(stack, NEL(items), sizeof(items[0]), items)
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* STACK_H */
