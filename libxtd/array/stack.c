/*
 * STACK.C --A simple stack container, with caller provided item storage.
 */
#include <memory.h>
#include <stack.h>

/*
 * stack_alloc() --Allocate some space for a stack structure.
 *
 * Returns: (StackPtr)
 * Success: the allocated memory; Failure: NULL.
 */
/* LCOV_EXCL_START */
StackPtr stack_alloc()
{
    return malloc(sizeof(Stack));
}
/* LCOV_EXCL_STOP */

/*
 * stack_init() --Initialise a stack structure.
 *
 * Parameters:
 * stack --specifies and returns the initialised stack
 * n_items --the number of stack items
 * item_size --the size of each item
 * items --the storage for the stack (n_items*item_size)
 *
 * Returns:
 * The stack.
 *
 * Remarks:
 * The stack storage is not allocated by this module, it is provided
 * by the caller.
 */
StackPtr stack_init(StackPtr stack, size_t n_items, size_t item_size, void *items)
{
    if (stack != NULL && items != NULL)
    {
        stack->container.n_items = n_items;
        stack->container.item_size = item_size;
        stack->container.items = (char *) items;
        stack->position = 0;
        return stack;
    }
    return NULL;
}

/*
 * stack_push() --Push an item onto the stack.
 *
 * Parameters:
 * stack --the stack
 * item --the item to push
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int stack_push(StackPtr stack, const void *item)
{
    if (stack != NULL)
    {
        if (stack->position < stack->container.n_items)
        {
            memcpy(stack->container.items +
                   stack->position*stack->container.item_size,
                   item, stack->container.item_size);
            ++stack->position;
            return 1;                   /* success */
        }

    }
    return 0;                           /* failure: no stack, or overflow */
}

/*
 * stack_pop() --Pop an item from the stack.
 *
 * Parameters:
 * stack --the stack
 * item --returns the popped item
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int stack_pop(StackPtr stack, void *item)
{
    if (stack != NULL)
    {
        if (stack->position > 0)
        {
            --stack->position;
            memcpy(item,
                   stack->container.items +
                   stack->position*stack->container.item_size,
                   stack->container.item_size);
            return 1;                   /* success */
        }

    }
    return 0;                           /* failure: no stack, or underflow */
}

/*
 * stack_peek() --Peek at the top item in the stack.
 *
 * Parameters:
 * stack --the stack
 *
 * Returns: (void *)
 * Success: a pointer to the top of stack; Failure: NULL.
 *
 * Remarks:
 * Consider allowing peek at lower stack items too?
 */
void *stack_peek(StackPtr stack)
{
    if (stack != NULL)
    {
        if (stack->position > 0)
        {
            return stack->container.items + /* success: return pointer */
                (stack->position-1)*stack->container.item_size;
        }
    }
    return NULL;                           /* failure: no stack, or empty */
}
