/*
 * STACK.C --A simple stack array, with caller provided item storage.
 *
 * Contents:
 * stack_alloc() --Allocate some space for a stack structure.
 * stack_init()  --Initialise a stack structure.
 * stack_push()  --Push an item onto the stack.
 * stack_pop()   --Pop an item from the stack.
 * stack_peek()  --Peek at the top item in the stack.
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
StackPtr stack_init(StackPtr stack, size_t n_items, size_t item_size,
                    void *items)
{
    if (stack != NULL && items != NULL)
    {
        stack->array.n_items = n_items;
        stack->array.item_size = item_size;
        stack->array.items = (char *) items;
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
        if (stack->position < stack->array.n_items)
        {
            memcpy(stack->array.items +
                   stack->position * stack->array.item_size,
                   item, stack->array.item_size);
            ++stack->position;
            return 1;                  /* success */
        }

    }
    return 0;                          /* failure: no stack, or overflow */
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
    if (stack_peek(stack, item))
    {
        --stack->position;
        return 1;                       /* success */
    }
    return 0;                          /* failure: no stack, or underflow */
}

/*
 * stack_peek() --Peek at the top item in the stack.
 *
 * Parameters:
 * stack --the stack
 * item --NULL, or the address to copy the top item.
 *
 * Returns: (void *)
 * Success: a pointer to the top of stack; Failure: NULL.
 *
 * Remarks:
 * REVISIT: Consider allowing peek at lower stack items too?
 */
void *stack_peek(StackPtr stack, void *item)
{
    char *stack_item = NULL;

    if (stack != NULL && stack->position > 0)
    {
        stack_item =
            stack->array.items +
            (stack->position-1) * stack->array.item_size;
        if (item != NULL)
        {
            memcpy(item, stack_item, stack->array.item_size);
        }
        return stack_item;          /* success: return pointer */
    }
    return NULL;                       /* failure: no stack, or empty */
}
