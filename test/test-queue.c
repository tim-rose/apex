/*
 * TEST-QUEUE.C --Unit tests for the queue structure.
 *
 * Remarks:
 * TBD
 */
#include <stdio.h>
#include <string.h>

#include <xtd/tap.h>
#include <xtd/test.h>
#include <xtd/queue.h>

static void test_null(void);
static void test_mask(void);
static void test_int(int n);

int main(void)
{
    plan_tests(28);
    test_mask();
    test_null();
    test_int(1);
    test_int(8);

    return exit_status();
}

/*
 * test_null() --Test that NULL queues are handled well.
 *
 * Parameters:
 * n	--the size of the queue to test.
 */
static void test_null(void)
{
    char item;
    AtomicQueue queue;

    diag("%s()", __func__);
    ok(queue_init(NULL, 1, 1, NULL) == NULL,
       "cannot initialise a NULL queue");
    number_eq(queue_push(NULL, &item), 0, "%d",
           "cannot push to a NULL queue");
    number_eq(queue_pop(NULL, &item), 0, "%d",
           "cannot pop from a NULL queue");
    ok(queue_peek(NULL, NULL) == NULL,
       "cannot peek at a NULL queue");
    ok(queue_init(&queue, 3, 1, &item) == NULL,
       "cannot initialise a queue with invalid size");
    ok(queue_init(&queue, 1, 1, NULL) == NULL,
       "cannot initialise a queue with no storage");
}


/*
 * test_mask() --Test queue mask generator.
 */
static void test_mask(void)
{
    int status;
    int mask;

    status = queue_mask(1, &mask);
    number_eq(status, 1, "%d", "size 1 is valid");
    number_eq(mask, 0x00, "%d", "calculated mask is correct?");

    status = queue_mask(2, &mask);
    number_eq(status, 1, "%d", "size 2 is valid");
    number_eq(mask, 0x01, "%d", "calculated mask is correct");

    status = queue_mask(3, &mask);
    number_eq(status, 0, "%d", "size 3 is not valid");

    status = queue_mask(5, &mask);
    number_eq(status, 0, "%d", "size 5 is not valid");

    status = queue_mask(4, &mask);
    number_eq(status, 1, "%d", "size 4 is valid");
    number_eq(mask, 0x03, "%d", "calculated mask is correct");
}



/*
 * test_int() --Test a queue of n integer-sized items.
 *
 * Parameters:
 * n	--the size of the queue to test.
 */
static void test_int(int n)
{
    diag("%s(%d)", __func__, n);

    int status = 1;
    int storage[n];
    int item = n+1;
    AtomicQueue queue;
    AtomicQueuePtr q = init_queue(&queue, storage);

    ok(q == &queue, "init_queue() returns first argument");
    number_eq(queue_pop(q, &item), 0, "%d",
           "queue_pop() underflow: returns failure");
    ok(queue_peek(q, NULL) == NULL,
       "queue_peek() underflow: returns NULL");

    for (int i = 0; i < n; ++i)
    {                                   /* fully load up queue */
        int push_ok = queue_push(q, &i);

        if (!push_ok)
        {
            status = 0;
        }
    }
    ok(status, "queue_push() all items");
    if (n > 0)
    {
        void *address = queue_peek(q, NULL);
        ok(address >= (void *) storage && address < (void*) (storage+n),
           "queue_peek() returns address from storage");
    }
    else
    {
        ok(queue_peek(q, NULL) == NULL,
           "queue_peek() underflow returns NULL");
    }

    number_eq(queue_push(q, &item), 0, "%d",
           "queue_push() overflow returns failure");

    status = 1;
    for (int i = 0; i < n; ++i)
    {                                   /* fully drain queue */
        int pop_ok = queue_pop(q, &item);

        if (!pop_ok)
        {
            diag("queue_pop[%d] fails", i);
            status = 0;
        }
        if (item != i)
        {
            diag("queue_pop[%d] bad value. Got %d, expected %d.",
                 i, item, i);
            status = 0;
        }
    }
    ok(status, "queue_pop() returns correct items");
}
