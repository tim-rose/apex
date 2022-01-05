/*
 * TEST-STACK.C --Unit tests for the stack structure.
 *
 * Remarks:
 * TBD
 */
#include <stdio.h>
#include <string.h>

#include <tap.h>
#include <test.h>
#include <stack.h>

/*
 * test_null() --Test that NULL stacks are handled well.
 *
 * Parameters:
 * n	--the size of the stack to test.
 */
static void test_null(void)
{
    char item;
    Stack stack;

    diag("%s()", __func__);
    ok(stack_init(NULL, 10, 1, NULL) == NULL,
       "cannot initialise a NULL stack");
    ok(stack_init(&stack, 10, 1, NULL) == NULL,
       "cannot initialise a stack with no storage");
    int_eq(stack_push(NULL, &item), 0, "%d",
       "cannot push to a NULL stack");
    int_eq(stack_pop(NULL, &item), 0, "%d",
       "cannot pop from a NULL stack");
    ok(stack_peek(NULL) == NULL,
       "cannot peek at a NULL stack");
}


/*
 * test_char() --Test a stack of n character-sized items.
 *
 * Parameters:
 * n	--the size of the stack to test.
 */
static void test_char(int n)
{
    diag("%s(%d)", __func__, n);

    char storage[n];
    char item = n+1;
    Stack stack;
    StackPtr s = init_stack(&stack, storage);
    int status = 1;

    ok(s == &stack, "init_stack() returns first argument");
    int_eq(stack_pop(s, &item), 0, "%d",
           "stack_pop() underflow returns failure");
    ok(stack_peek(s) == NULL,
           "stack_peek() underflow returns NULL");

    for (int i = 0; i < n; ++i)
    {                                   /* fully load up stack */
        char x = i + 1;
        int push_ok = stack_push(s, &x);

        if (!push_ok)
        {
            status = 0;
        }
    }
    ok(status, "stack_push() all items");
    if (n > 0)
    {
        ok(stack_peek(s) == &storage[n-1],
           "stack_peek() returns address in storage");
    }
    else
    {
        ok(stack_peek(s) == NULL,
           "stack_peek() underflow returns NULL");
    }

    int_eq(stack_push(s, &item), 0, "%d",
           "stack_push() overflow returns failure");

    status = 1;
    for (int i = 0; i < n; ++i)
    {                                   /* fully drain stack */
        char x = i;
        int pop_ok = stack_pop(s, &x);

        if (!pop_ok)
        {
            diag("stack_pop[%d] fails", i);
            status = 0;
        }
        if (x != n - i)
        {
            diag("stack_pop[%d] wrong value. Got %d, expected %d.",
                 i, x, n - i);
            status = 0;
        }
    }
    ok(status, "stack_pop() returns correct items");
}

/*
 * test_int() --Test a stack of n integer-sized items.
 *
 * Parameters:
 * n	--the size of the stack to test.
 */
static void test_int(int n)
{
    diag("%s(%d)", __func__, n);

    int storage[n];
    int item = n+1;
    Stack stack;
    StackPtr s = init_stack(&stack, storage);
    int status = 1;

    ok(s == &stack, "init_stack() returns first argument");
    int_eq(stack_pop(s, &item), 0, "%d",
           "stack_pop() underflow returns failure");
    ok(stack_peek(s) == NULL,
           "stack_peek() underflow returns NULL");

    for (int i = 0; i < n; ++i)
    {                                   /* fully load up stack */
        int x = i + 1;
        int push_ok = stack_push(s, &x);

        if (!push_ok)
        {
            status = 0;
        }
    }
    ok(status, "stack_push() all items");
    if (n > 0)
    {
        ok(stack_peek(s) == &storage[n-1],
           "stack_peek() returns address in storage");
    }
    else
    {
        ok(stack_peek(s) == NULL,
           "stack_peek() underflow returns NULL");
    }

    int_eq(stack_push(s, &item), 0, "%d",
           "stack_push() overflow returns failure");

    status = 1;
    for (int i = 0; i < n; ++i)
    {                                   /* fully drain stack */
        int x = i;
        int pop_ok = stack_pop(s, &x);

        if (!pop_ok)
        {
            diag("stack_pop[%d] fails", i);
            status = 0;
        }
        if (x != n - i)
        {
            diag("stack_pop[%d] wrong value. Got %d, expected %d.",
                 i, x, n - i);
            status = 0;
        }
    }
    ok(status, "stack_pop() returns correct items");
}


int main(void)
{
    plan_tests(33);
    test_null();
    test_char(0);
    test_char(1);
    test_char(10);
    test_int(10);

    return exit_status();
}
