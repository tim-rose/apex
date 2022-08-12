/*
 * HASH-TEST.C --Unit tests for the hash-table module.
 *
 * Contents:
 * hash()         --A dummy hash function that makes tests easier.
 * print_item()   --Print a hash item into the global text string.
 * compare_item() --Compare two hash items for equality.
 *
 */
#include <stdio.h>
#include <string.h>
#include <xtd.h>
#include <xtd/tap.h>
#include <xtd/log.h>
#include <xtd/hash.h>

static char text[100];

/*
 * hash() --A dummy hash function that makes tests easier.
 */
static unsigned long hash(char *data)
{
    return (unsigned long) data;
}

/*
 * print_item() --Print a hash item into the global text string.
 */
static void *print_item(void *data, void *UNUSED(usrdata))
{
    char databuf[20];

    sprintf(databuf, "%d ", (int) data);
    strcat(text, databuf);
    return 0;
}

/*
 * compare_item() --Compare two hash items for equality.
 *
 * Remarks:
 * This routine returns values in a strcmp()-like manner.
 */
static int compare_item(const void *data, const void *key)
{
    return ((int) data - (int) key);
}

/*
 * These tests rely on having a "dumb" hash function to order
 * the items in the slot in a particular way.
 */
int main(void)
{
    HashPtr h;

    plan_tests(10);

    ok((int) hash_new(hash, 0) == 0, "hash_new() bad nslots");

    h = hash_new(hash, 2);
    ok((int) h != 0, "hash_new() two nslots");

    hash_insert(h, (void *) 1);
    hash_visit(h, print_item, 0);
    ok(strcmp(text, "1 ") == 0, "hash_visit() one item");

    text[0] = '\0';
    hash_insert(h, (void *) 2);
    hash_visit(h, print_item, 0);
    ok(strcmp(text, "2 1 ") == 0, "hash_visit() two items");

    text[0] = '\0';
    hash_insert(h, (void *) 3);
    hash_insert(h, (void *) 4);
    hash_visit(h, print_item, 0);
    ok(strcmp(text, "4 2 3 1 ") == 0, "hash_visit() four items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 3);
    hash_visit(h, print_item, 0);
    ok(strcmp(text, "4 2 1 ") == 0, "hash_remove() three items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 1);
    hash_visit(h, print_item, 0);
    ok(strcmp(text, "4 2 ") == 0, "hash_remove() two items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 2);
    hash_visit(h, print_item, 0);
    ok(strcmp(text, "4 ") == 0, "hash_remove() one items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 4);
    hash_visit(h, print_item, 0);
    ok(strcmp(text, "") == 0, "hash_remove() zero items");

    ok(hash_remove(h, compare_item, (void *) 0) == 0,
       "hash_remove() non-existent item");

    return exit_status();
}
