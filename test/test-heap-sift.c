/*
 * HEAP.C --heap library tests.
 *
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
#include <heap.h>

#define HEAP_MAX 10
static int cmp_number(int *n1, int *n2)
{
    /* printf("# cmp_number: %d, %d\n", *n1, *n2); */
    return *n1 - *n2;
}

static int heap_print(int value[], int n)
{
    printf("# heap:");
    for (int i = 0; i < n; ++i)
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
    for (size_t i = 0; i < HEAP_MAX; ++i)
    {                                  /* insert in sorted order */
        value[i] = sorted[i];
        heap_sift_up(value, i + 1, sizeof(int), (CompareProc) cmp_number);
        if (!ok(heap_ok(value, i + 1, sizeof(int), (CompareProc) cmp_number),
                "insert sorted %d", i + 1))
        {
            heap_print(value, i + 1);
        }
    }

    for (size_t i = HEAP_MAX - 1; i > 0; --i)
    {
        value[0] = value[i];
        heap_sift_down(value, i, sizeof(int), (CompareProc) cmp_number);
        if (!ok(heap_ok(value, i, sizeof(int), (CompareProc) cmp_number),
                "delete sorted %d", i))
        {
            heap_print(value, i);
        }
    }

    for (size_t i = 0; i < HEAP_MAX; ++i)
    {                                  /* insert in reverse order */
        value[i] = reverse[i];
        heap_sift_up(value, i + 1, sizeof(int), (CompareProc) cmp_number);
        if (!ok(heap_ok(value, i + 1, sizeof(int), (CompareProc) cmp_number),
                "insert reverse %d", i + 1))
        {
            heap_print(value, i + 1);
        }
    }

    for (size_t i = HEAP_MAX - 1; i > 0; --i)
    {
        value[0] = value[i];
        heap_sift_down(value, i, sizeof(int), (CompareProc) cmp_number);
        if (!ok(heap_ok(value, i, sizeof(int), (CompareProc) cmp_number),
                "delete reverse %d", i))
        {
            heap_print(value, i);
        }
    }
    return exit_status();
}
