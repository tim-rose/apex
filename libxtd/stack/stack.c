/*
 * STACK.C --A simple stack container, with caller provided item storage.
 */
#include <memory.h>
#include <stack.h>

StackPtr stack_alloc()
{
    return malloc(sizeof(Stack));
}

StackPtr stack_init(StackPtr stack, size_t n_items, size_t item_size, void *items)
{
    if (stack != NULL)
    {
        stack->container.n_items = n_items;
        stack->container.item_size = item_size;
        stack->container.items = (char *) items;
        stack->position = 0;
    }
    return stack;                       /* NULL cascades to caller */
}

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

void *stack_peek(StackPtr stack)
{
    if (stack != NULL)
    {
        if (stack->position > 0)
        {
            return stack->container.items +
                (stack->position-1)*stack->container.item_size;
        }
    }
    return NULL;                           /* failure: no stack, or empty */
}
