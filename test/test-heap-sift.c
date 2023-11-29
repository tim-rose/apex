/*
 * TEST-HEAP-SIFT.C --Tests for heap fundamental operations.
 *
 * Remarks:
 * I'm not sure how rigorous these tests are, but thry attempt
 * to cover the basic cases by enumeration.
 */
#include <stdio.h>
#include <string.h>

#include <xtd/tap.h>
#include <xtd/heap.h>
#include <xtd/compare.h>

static void test_heap_ok(void);
static void test_sift_up(void);
static void test_sift_down(void);


int main(void)
{
    plan_tests(18);

    test_heap_ok();
    test_sift_up();
    test_sift_down();

    return exit_status();
}

#if 0                                  /* debugging... */
static void print_heap(int heap[], size_t n)
{
    char text[100];
    char *str = text;

    for (size_t i = 0; i < n; ++i)
    {
        str += sprintf(str, " %d", heap[i]);
    }
    diag("heap:%s", text);
}
#endif /* 0 */

static void test_heap_ok(void)
{
    const char func[] = "heap_ok";
    int trivial_heap[] = { 1 };
    int ok_heap_2[] = { 1, 2 };
    int bad_heap_2[] = { 2, 1 };
    int ok_heap_3a[] = { 1, 2, 3 };
    int ok_heap_3b[] = { 1, 3, 2 };
    int bad_heap_3a[] = { 2, 1, 3 };
    int bad_heap_3b[] = { 2, 3, 1 };
    int bad_heap_3c[] = { 3, 1, 2 };
    int bad_heap_3d[] = { 3, 2, 1 };

#define HEAP_OK_CHECK(h) \
    ok(heap_ok(h, NEL(h), sizeof(h[0]), int_cmp), \
       "%s %s %s", func, "accepts", #h)
#define HEAP_ERR_CHECK(h) \
    ok(!heap_ok(h, NEL(h), sizeof(h[0]), int_cmp), \
       "%s %s %s", func, "rejects", #h)

    HEAP_OK_CHECK(trivial_heap);
    HEAP_OK_CHECK(ok_heap_2);
    HEAP_ERR_CHECK(bad_heap_2);

    HEAP_OK_CHECK(ok_heap_3a);
    HEAP_OK_CHECK(ok_heap_3b);
    HEAP_ERR_CHECK(bad_heap_3a);
    HEAP_ERR_CHECK(bad_heap_3b);
    HEAP_ERR_CHECK(bad_heap_3c);
    HEAP_ERR_CHECK(bad_heap_3d);
}

static void test_sift_up(void)
{
    const char func[] = "heap_sift_up";
    int heap_2[] = { 2, 1 };
    int heap_3a[] = { 2, 3, 1 };
    int heap_3b[] = { 3, 2, 1 };
    int heap_3c[] = { 1, 2, 3 };

#define HEAP_SIFT_UP_CHECK(h) \
    heap_sift_up(h, NEL(h), sizeof(h[0]), int_cmp);     \
    ok(heap_ok(h, NEL(h), sizeof(h[0]), int_cmp), \
       "%s %s %s", func, "sifts up", #h)

    HEAP_SIFT_UP_CHECK(heap_2);
    HEAP_SIFT_UP_CHECK(heap_3a);
    HEAP_SIFT_UP_CHECK(heap_3b);
    HEAP_SIFT_UP_CHECK(heap_3c);
}

static void test_sift_down(void)
{
    const char func[] = "heap_sift_down";
    int heap_1[] = { 1 };
    int heap_2[] = { 2, 1 };
    int heap_3a[] = { 2, 3, 1 };
    int heap_3b[] = { 1, 3, 2 };
    int heap_3c[] = { 1, 2, 3 };

#define HEAP_SIFT_DOWN_CHECK(h) \
    heap_sift_down(h, 0, NEL(h), sizeof(h[0]), int_cmp); \
    ok(heap_ok(h, NEL(h), sizeof(h[0]), int_cmp), \
       "%s %s %s", func, "sifts down", #h)

    HEAP_SIFT_DOWN_CHECK(heap_1);
    HEAP_SIFT_DOWN_CHECK(heap_2);
    HEAP_SIFT_DOWN_CHECK(heap_3a);
    HEAP_SIFT_DOWN_CHECK(heap_3b);
    HEAP_SIFT_DOWN_CHECK(heap_3c);
}
