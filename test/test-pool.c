/*
 * TEST-POOL.C --Unit tests for the pool allocator.
 *
 * Remarks:
 * TBD
 */
#include <stdio.h>
#include <string.h>

#include <apex/tap.h>
#include <apex/test.h>
#include <apex/pool.h>

static void test_null(void);
static void test_pool(int n, int prealloc);

int main(void)
{
    plan_tests(18);
    test_null();
    test_pool(1, 0);
    test_pool(10, 0);
    test_pool(10, 5);

    return exit_status();
}

/*
 * test_null() --Test that NULL pools are handled well.
 *
 * Parameters:
 * n    --the size of the pool to test.
 */
static void test_null(void)
{
    char bad_items[10];
    void *ok_items[10];
    Pool pool, *pool_ptr;
    void *item;

    diag("%s()", __func__);
    pool_ptr = pool_init(NULL, NEL(ok_items), sizeof(ok_items[0]), ok_items);
    ptr_eq(pool_ptr, NULL, "cannot initialise a NULL pool");

    pool_ptr = pool_init(&pool, NEL(ok_items), sizeof(ok_items[0]), NULL);
    ptr_eq(pool_ptr, NULL, "cannot initialise a pool with no storage");

    pool_ptr = pool_init(&pool, 0, sizeof(ok_items[0]), ok_items);
    ptr_eq(pool_ptr, NULL, "cannot initialise a pool with no items?");

    pool_ptr =
        pool_init(&pool, NEL(bad_items), sizeof(bad_items[0]), bad_items);
    ptr_eq(pool_ptr, NULL, "cannot initialise a pool with bad item size");

    item = pool_new(NULL);
    ptr_eq(item, NULL, "pool_new() fails on a NULL pool");

    pool_delete(NULL, NULL);
    pass("pool_delete() fails silently...");

}

/*
 * test_pool() --Test a pool of n items.
 *
 * Parameters:
 * n    --the size of the pool to test.
 */
static void test_pool(int n, int prealloc)
{
    char *storage[n];
    Pool pool;
    Pool *p = init_pool(&pool, storage);
    int status = 1;
    void *item = NULL;

    diag("preallocating %d items", prealloc);
    for (int i = 0; i < prealloc; ++i)
    {
        item = pool_new(p);            /* discard */
    }

    ptr_eq(p, &pool, "init_pool() returns first argument");
    for (int i = 0; i < n * 2; ++i)
    {
        item = pool_new(p);
        if (item == NULL)
        {
            diag("pool_new(%d) failed", i);
            status = 0;
        }
        pool_delete(p, item);
    }
    ok(status, "consecutive new/deletes succeed");

    status = 1;
    for (int i = prealloc; i < n; ++i)
    {                                  /* drain pool */
        item = pool_new(p);            /* discard */
        if (item == NULL)
        {
            diag("pool_new(%d) failed", i);
            status = 0;
        }
    }
    ok(status, "drained all items from pool");

    item = pool_new(p);
    ptr_eq(item, NULL, "pool_new() fails on empty pool");
}
