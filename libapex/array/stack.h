/*
 * STACK.H --A simple stack array, with caller provided item storage.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 */
#ifndef APEX_STACK_H
#define APEX_STACK_H

#include <apex/array.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct Stack
    {
        ArrayContainer array;
        size_t top;                       /* current top of stack */
    } Stack;

    Stack *stack_alloc(void);
    Stack *stack_init(Stack *stack, size_t n_items,
                        size_t item_size, void *items);

    int stack_push(Stack *stack, const void *item);
    int stack_pop(Stack *stack, void *item);
    void *stack_peek(Stack *stack, void *item);


    /*
     * Convenience functions for malloc and item-size aware initialisation.
     */
#define new_stack(items) init_stack(stack_alloc(), items)
#define init_stack(stack, items) stack_init(stack, NEL(items), sizeof(items[0]), items)
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_STACK_H */
