/*
 * TEST-BINSEARCH.C --Unit tests for the binsearch function.
 */
#include <string.h>

#include <apex/test.h>
#include <apex/search.h>

int vector[] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };

static void test_zero_size_table(void)
{
    int key = 0;
    int slot;
    bool status;

    slot = binsearch(&key, vector, 0, sizeof(vector[0]),
                     (CompareProc) int_cmp, &status);
    number_eq(status, 0, "%d", "zero-size table: status is 0");
    number_eq(slot, 0, "%d", "zero-size table: slot is 0");
}

int main(void)
{
    int i, n, key = 0;
    int slot;
    bool status;

    plan_tests(36);

    test_zero_size_table();

    for (n = 0; n <= 10; n += 5)
    {
        for (i = 0; i < n; i += 2)
        {
            key = i;
            slot = binsearch(&key, vector, n, sizeof(vector[0]),
                             (CompareProc) int_cmp, &status);
            ok_number(status, !=, 0, "%d", "find key %d in vector[%d]: status", key, n);
            ok_number(slot, ==, key/2, "%d", "find key %d in vector[%d]: slot", key, n);

            key = i + 1;
            slot = binsearch(&key, vector, n, sizeof(vector[0]),
                             (CompareProc) int_cmp, &status);
            ok_number(status, ==, 0, "%d", "failed key %d in vector[%d]: status", key, n);
            ok_number(slot, ==, key/2+1, "%d", "find key %d in vector[%d]: slot", key, n);
        }
    }
    key = -1;
    slot = binsearch(&key, vector, 10, sizeof(vector[0]),
                     (CompareProc) int_cmp, &status);
    ok_number(status, ==, 0, "%d", "failed key %d in vector[%d]: status", key, 10);
    ok_number(slot, ==, 0, "%d", "find key %d in vector[%d]: slot", key, 10);
    return exit_status();
}
