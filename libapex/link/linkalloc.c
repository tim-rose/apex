/*
 * LINKALLOC.C --Linked-list allocation routines.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Contents:
 * link_block_new()  --Allocate a block of links.
 * link_new()        --Get a new link record and initialise it for the caller.
 * link_free()       --Return a link record to the free-list.
 * link_free_links() --Return an entire list of links to the free-list.
 *
 */
#include <stdlib.h>

#include <apex.h>
#include <apex/clink.h>


#define LINK_BLOCK	((4096-24)/sizeof(Link))

static Link *free_list;              /* tail of the (circular) free list */

/*
 * link_block_new() --Allocate a block of links.
 *
 * Returns: (Link *)
 * Success: the tail of a circular list of links; Failure: NULL.
 *
 * Remarks:
 * This routine is used to allocate new links in bulk so as to avoid
 * a per-link malloc overhead.
 */
static Link *_link_block_new(void)
{
    Link *l = (Link *) malloc(LINK_BLOCK * sizeof(Link));

    if (l)
    {
        for (size_t i = 0; i < LINK_BLOCK; ++i)
        {                              /* create a circular list */
            l[i].next = &l[(i + 1) % LINK_BLOCK];
        }
    }
    return l;
}

/*
 * link_new() --Get a new link record and initialise it for the caller.
 *
 * Parameters:
 * next --ptr to the next link in the chain.
 * value    --the thing being linked.
 *
 * Returns: (Link *)
 * Success: an initialised Link record; Failure: NULL (malloc failure).
 *
 * Remarks:
 * This calls _link_block_new() as necessary to renew the free-list.
 */
Link *link_new(Link *next, void *value)
{
    Link *new;

    if (free_list == NULL && (free_list = _link_block_new()) == NULL)
    {
        return NULL;                   /* failure: malloc failed */
    }
    new = free_list->next;
    if (new == free_list)
    {                                  /* that was the last link! */
        free_list = (Link *) NULL;
    }
    else
    {
        free_list->next = new->next;
    }
    new->next = next;
    new->data = value;
    return new;                        /* success */
}

/*
 * link_free() --Return a link record to the free-list.
 *
 * Parameters:
 * l    --the link to be freed.
 */
void link_free(Link *l)
{
    if (l == NULL)
    {
        return;
    }

    if (free_list)
    {
        l->next = free_list->next;
    }
    else
    {
        free_list = l;
    }
    free_list->next = l;
}

/*
 * link_free_links() --Return an entire list of links to the free-list.
 *
 * Parameter:
 * tail --the tail of the list to be freed.
 */
void link_free_links(Link *head, Link *tail)
{
    if (head == NULL || tail == NULL)
    {
        return;
    }
    if (free_list)
    {                                  /* splice in this list */
        tail->next = free_list->next;
        free_list->next = head;
    }
    else
    {                                  /* no free-list? */
        free_list = tail;              /* ...tail becomes entire free-list */
        tail->next = head;             /* make sure it's circular */
    }
}
