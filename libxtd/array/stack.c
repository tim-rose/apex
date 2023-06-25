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
#include <xtd/stack.h>

/*
 * stack_alloc() --Allocate some space for a stack structure.
 *
 * Returns: (StackPtr)
 * Success: the allocated memory; Failure: NULL.
 */
/* LCOV_EXCL_START */
StackPtr stack_alloc(void)
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
 * base --the storage for the stack (n_items*item_size)
 *
 * Returns:
 * The stack.
 *
 * Remarks:
 * The stack storage is not allocated by this module, it is provided
 * by the caller.
 */
StackPtr stack_init(StackPtr stack, int n_items, int item_size,
                    void *base)
{
    if (stack != NULL && base != NULL)
    {
        array_init(&stack->array, n_items, item_size, base);
        stack->top = 0;
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
        if (stack->top < stack->array.n_items)
        {
            memcpy(array_item(&stack->array, (int) stack->top),
                   item, stack->array.item_size);
            ++stack->top;
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
        --stack->top;
        return 1;                      /* success */
    }
    return 0;                          /* failure: no stack, or underflow */
}

/*
 * stack_peek() --Peek at the top item in the stack.
 *
 * Parameters:
 * stack --the stack
 * item --NULL, or returns a copy of the top item.
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

    if (stack != NULL && stack->top > 0)
    {
        stack_item = array_item(&stack->array, (int) stack->top - 1);

        if (item != NULL)
        {
            memcpy(item, stack_item, stack->array.item_size);
        }
        return stack_item;             /* success: return pointer */
    }
    return NULL;                       /* failure: no stack, or empty */
}
