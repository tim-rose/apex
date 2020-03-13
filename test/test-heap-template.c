/*
 * HEAP-TEMPLATE.C --heap template library tests.
 *
 * Remarks:
 * This is a bit of a scatter-gun approach to testing; I'm just running
 * some typical data through and hoping that this gives good code
 * coverage.  It PROBABLY does, but I'm sure I can whittle this down
 * to a much smaller set of more rigorous tests.
 */
#include <stdio.h>
#include <string.h>

#include <tap.h>
#include <heap-template.h>

#define cmp_number(_n1_, _n2_) (*(_n1_) - *(_n2_))
HEAP_SIFT_UP_PROC(int_sift_up, int, cmp_number)
HEAP_SIFT_DOWN_PROC(int_sift_down, int, cmp_number)
HEAP_OK_PROC(int_heap_ok, int, cmp_number)
     static int heap_print(int value[], size_t n)
{
    printf("# heap:");
    for (size_t i = 0; i < n; ++i)
    {
        printf(" %d", value[i]);
    }
    printf("\n");
    return 1;
}

int main(void)
{
    int reverse[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    int sorted[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int value[10];

    plan_tests(38);
    for (size_t i = 0; i < 10; ++i)
    {                                  /* insert in sorted order */
        value[i] = sorted[i];
        int_sift_up(value, i + 1);
        if (!ok(int_heap_ok(value, i + 1), "insert sorted %d", i + 1))
        {
            heap_print(value, i + 1);
        }
    }
    for (unsigned int i = 10 - 1; i > 0; --i)
    {
        value[0] = value[i];
        int_sift_down(value, i);
        if (!ok(int_heap_ok(value, i), "delete sorted %d", i))
        {
            heap_print(value, i);
        }
    }
    for (size_t i = 0; i < 10; ++i)
    {                                  /* insert in reverse order */
        value[i] = reverse[i];
        int_sift_up(value, i + 1);
        if (!ok(int_heap_ok(value, i + 1), "insert reverse %d", i + 1))
        {
            heap_print(value, i + 1);
        }
    }
    for (size_t i = 10 - 1; i > 0; --i)
    {
        value[0] = value[i];
        int_sift_down(value, i);
        if (!ok(int_heap_ok(value, i), "delete reverse %d", i))
        {
            heap_print(value, i);
        }
    }
    return exit_status();
}
