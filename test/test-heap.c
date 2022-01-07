/*
 * TEST-HEAP.C --Unit tests for the heap structure.
 *
 * Remarks:
 * TBD
 */
#include <stdio.h>
#include <string.h>

#include <tap.h>
#include <test.h>
#include <heap.h>
#include <compare.h>

static void print_heap(HeapPtr heap);
static void test_null(void);
static void test_int(int n);

int main(void)
{
    plan_tests(26);
    test_null();
    test_int(0);
    test_int(1);
    test_int(10);

    return exit_status();
}

static void print_heap(HeapPtr heap)
{
    char text[100];
    char *str = text;

    diag("heap: n_items=%lu, item_size=%lu, n_used=%lu",
         heap->array.n_items,
         heap->array.item_size,
         heap->n_used);
    for (size_t i=0; i<heap->n_used; ++i)
    {
        int item;

        memcpy(&item,
               heap->array.items + i*heap->array.item_size,
               heap->array.item_size);

        str += sprintf(str, " %d", item);
    }
    diag("array:%s", text);
}


/*
 * test_null() --Test that NULL heaps are handled well.
 */
static void test_null(void)
{
    char item;
    Heap heap;

    diag("%s()", __func__);
    ok(heap_init(NULL, int_cmp, 10, 1, NULL) == NULL,
       "cannot initialise a NULL heap");
    ok(heap_init(&heap, int_cmp, 10, 1, NULL) == NULL,
       "cannot initialise a heap with no storage");
    int_eq(heap_push(NULL, &item), 0, "%d",
       "cannot push to a NULL heap");
    int_eq(heap_pop(NULL, &item), 0, "%d",
       "cannot pop from a NULL heap");
    ok(heap_peek(NULL, NULL) == NULL,
       "cannot peek at a NULL heap");
}

/*
 * test_int() --Test a heap of n int-sized items.
 *
 * Parameters:
 * n	--the size of the heap to test.
 *
 * Remarks:
 * This tests some boundary conditions, and then inserts
 * a sequence in reverse order, then pops the values out,
 * noting that they are returned in ordered sequence.
 */
static void test_int(int n)
{
    diag("%s(%d)", __func__, n);

    int storage[n];
    int item;
    Heap heap = {0};
    HeapPtr h = init_heap(&heap, int_cmp, storage);
    int status = 1;

    ok(h == &heap, "init_heap() returns first argument");
    int_eq(heap_pop(h, &item), 0, "%d",
           "heap_pop() underflow returns failure");
    ok(heap_peek(h, NULL) == NULL,
           "heap_peek() underflow returns NULL");

    for (int i = 0; i < n; ++i)
    {                                   /* fully load up heap */
        item = n - i;
        int push_ok = heap_push(h, &item);

        if (!push_ok)
        {
            status = 0;
        }
    }
    ok(status, "heap_push() all items");
    if (n > 0)
    {
        ok(heap_peek(h, NULL) == &storage[0],
           "heap_peek() returns address in storage");
    }
    else
    {
        ok(heap_peek(h, NULL) == NULL,
           "heap_peek() underflow returns NULL");
    }

    int_eq(heap_push(h, &item), 0, "%d",
           "heap_push() overflow returns failure");

    status = 1;
    for (int i = 0; i < n; ++i)
    {                                   /* fully drain heap */
        int pop_ok = heap_pop(h, &item);

        if (!pop_ok)
        {
            diag("heap_pop[%d] fails", i);
            status = 0;
        }
        if (item != i+1)
        {
            diag("heap_pop[%d] bad value. Got %d, expected %d.",
                 i, item, i+1);
            status = 0;
        }
    }
    if (!ok(status, "heap_pop() returns correct items"))
    {
        print_heap(h);
    }
}
