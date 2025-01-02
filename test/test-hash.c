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
#include <apex.h>
#include <apex/tap.h>
#include <apex/test.h>
#include <apex/log.h>
#include <apex/hash.h>

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

    sprintf(databuf, "%ld ", (long int) data);
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
    return ((const char *) data - (const char *) key);
}

/*
 * These tests rely on having a "dumb" hash function to order
 * the items in the slot in a particular way.
 */
int main(void)
{
    Hash *h;

    plan_tests(10);

    ptr_eq(hash_new(hash, 0), NULL, "hash_new() bad nslots");

    h = hash_new(hash, 2);
    ok_number(h, !=,  NULL, "0x%p", "hash_new() two nslots");

    hash_insert(h, (void *) 1);
    hash_visit(h, print_item, 0);
    string_eq(text, "1 ", "hash_visit() one item");

    text[0] = '\0';
    hash_insert(h, (void *) 2);
    hash_visit(h, print_item, 0);
    string_eq(text, "2 1 ", "hash_visit() two items");

    text[0] = '\0';
    hash_insert(h, (void *) 3);
    hash_insert(h, (void *) 4);
    hash_visit(h, print_item, 0);
    string_eq(text, "4 2 3 1 ", "hash_visit() four items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 3);
    hash_visit(h, print_item, 0);
    string_eq(text, "4 2 1 ", "hash_remove() three items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 1);
    hash_visit(h, print_item, 0);
    string_eq(text, "4 2 ", "hash_remove() two items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 2);
    hash_visit(h, print_item, 0);
    string_eq(text, "4 ", "hash_remove() one items");

    text[0] = '\0';
    hash_remove(h, compare_item, (void *) 4);
    hash_visit(h, print_item, 0);
    string_eq(text, "", "hash_remove() zero items");

    ptr_eq(hash_remove(h, compare_item, (void *) 0), NULL,
       "hash_remove() non-existent item");

    return exit_status();
}
