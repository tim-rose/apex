/*
 * CLINK.C --Link circularly singly-linked lists.
 *
 * Contents:
 * clink_len()     --Calculate the length of a linked list.
 * clink_reverse() --Reverse a linked-list.
 * clink_rotate()  --Rotate a linked-list.
 * clink_add()     --Add a single link to the head of an existing linked list.
 * clink_insert()  --Insert an item into an ordered list.
 * clink_remove()  --Remove a single link from a linked-list.
 * clink_find()    --Find a particular item in a linked-list.
 * clink_visit()   --Visit all the items in a linked-list.
 *
 * Remarks:
 * The Link module provides a simple link structure that can be used
 * to manage singly-linked lists.  The implementation is somewhat
 * novel, and efficient.  By convention:
 *  * An empty list is represented as a NULL pointer
 *  * A non-empty list is represented as a pointer to the TAIL link of the list
 *  * The list is ALWAYS circular; i.e. the tail link always points to
 *    the head link (possibly the same link).
 *
 * This representation is optimal in the sense that many of the
 * special cases associated with managing head and tail pointers, and
 * managing a pointer to the list proper (as distinct from the list
 * itself) collapse into the general case, and thus don't require extra
 * tests and behaviour.
 *
 * Nevertheless, dealing with such lists can be confusing if you
 * are used to pointer-to-head styled lists.  Fortunately this is not
 * a problem in practice, as you can treat the Link record as
 * an ADT, and simply use the API provided by this module.
 *
 * Note that this implementation is minimal, and some operations
 * are expensive.  In particular:
 *  * clink_len() calculates the length by scanning the list (i.e.
 *    it takes O(n) time)
 *  * clink_rotate() rotates backwards by rotating n % clink_len() steps forward.
 */

#include <stdio.h>
#include <xtd.h>
#include <clink.h>

/*
 * clink_len() --Calculate the length of a linked list.
 *
 * Parameters:
 * tail --specifies the tail of the (circular) list
 *
 * Returns: (int)
 * The length of the list.
 */
int clink_len(LinkPtr tail)
{
    int n = 0;

    if (tail != NULL)
    {
        LinkPtr l = tail->next;

        for (n = 1; l != tail; ++n)
        {
            l = l->next;
        }
    }
    return n;
}

/*
 * clink_reverse() --Reverse a linked-list.
 *
 * Parameters:
 * tail --specifies the tail of the (circular) list to reversed
 *
 * Returns: (LinkPtr)
 * the tail of the reversed list.
 */
LinkPtr clink_reverse(LinkPtr tail)
{
    LinkPtr p, l, n;
    LinkPtr head;

    if (tail == NULL || tail->next == tail)
    {
        return tail;                   /* trivial */
    }
    p = tail;                          /* "previous" */
    head = l = tail->next;             /* head, initially */
    do
    {
        n = l->next;                   /* save */
        l->next = p;                   /* reverse this link */
        p = l;                         /* march through to next item */
        l = n;
    } while (l != head);
    return l;                          /* return head (==new tail!) */
}

/*
 * clink_rotate() --Rotate a linked-list.
 *
 * Parameters:
 * tail --specifies the tail of the (circular) list to be rotated
 * n    --specifies the amount to rotate
 *
 * Returns: (LinkPtr)
 * The tail of the rotated list.
 *
 * Remarks:
 * Positive values of 'n' rotate the list towards the tail
 * (clockwise!?).  negative values will rotate in the opposite
 * direction, but may be expensive.
 *
 * Another routine that permutes links is clink_reverse(), which
 * reverses the sequence of a list in-place.
 */
LinkPtr clink_rotate(LinkPtr tail, int n)
{
    if (tail != NULL && n != 0)
    {
        int i;

        if (n < 0)
        {                              /* Note: clink_len() is expensive */
            n = clink_len(tail) + n;
        }
        for (i = 0; i < n; ++i)
        {
            tail = tail->next;
        }
    }
    return tail;
}

/*
 * clink_add() --Add a single link to the head of an existing linked list.
 *
 * Parameters:
 * tail --the tail of the list to add this link
 * link --the link to add.
 *
 * Returns: (LinkPtr)
 * the tail of the modified list.
 */
LinkPtr clink_add(LinkPtr tail, LinkPtr link)
{
    if (tail == NULL)
    {
        tail = link;
    }
    link->next = tail->next;
    tail->next = link;

    return (tail);
}

/*
 * clink_insert() --Insert an item into an ordered list.
 *
 * Parameters:
 * tail --the tail of the list to add this link
 * cmp  --the comparison function used to match the item
 * value    --the value to insert
 * mode --the insertion mode (fail, replace, duplicate)
 *
 * Returns: (LinkPtr)
 * Success: the tail of the modified list; Failure: NULL.
 *
 * Parameter <code>mode</code> is used to handle the situation where
 * the value appears in the list already (as determined by <code>cmp()</code>).
 *
 * See Also: LinkInsertMode
 */
LinkPtr clink_insert(LinkPtr tail, CompareProc cmp, void *value,
                     LinkInsertMode mode)
{
    LinkPtr head = tail->next;
    LinkPtr l = head;
    LinkPtr prev = tail;
    LinkPtr n;
    int diff;

    do
    {                                  /* skip past lesser items */
        if ((diff = cmp(l->data, value)) >= 0)
        {
            break;
        }
        prev = l;
        l = l->next;
    } while (l != head);

    if (diff < 0)
    {                                  /* all items less: append to tail */
        if ((n = link_new(tail->next, value)) != NULL)
        {
            tail->next = n;
            return n;                  /* the new tail */
        }
    }
    else if (diff == 0)
    {
        switch (mode)
        {
        default:
        case LINK_INSERT_FAIL:
            return NULL;               /* failure under instruction */
        case LINK_INSERT_REPLACE:
            l->data = value;
            return tail;               /* no change to list */
        case LINK_INSERT_DUPLICATE:   /* insert after */
            if ((n = link_new(l->next, value)) != NULL)
            {                          /* may have a new tail */
                l->next = n;
                return (l == tail ? n : tail);
            }
        }
    }
    else                               /* diff > 0 */
    {
        if ((n = link_new(prev->next, value)) != NULL)
        {
            prev->next = n;
            return tail;
        }
    }
    return NULL;                       /* failure */
}

/*
 * clink_remove() --Remove a single link from a linked-list.
 *
 * Parameters:
 * tail --the tail of the list to add this link
 * cmp  --the comparison function used to match the item
 * key  --the key value used to compare items
 * remlink  --returns the link that was removed (if any)
 *
 * Returns: (LinkPtr)
 * the tail of the modified list; NULL if the list is empty.
 *
 * Remarks:
 * remlink will be set to NULL if the item is not found.
 */
LinkPtr clink_remove(LinkPtr tail, CompareProc cmp, void *key,
                     LinkPtr * remlink)
{
    if (tail != NULL)
    {
        LinkPtr head = tail->next;
        LinkPtr l = head;
        LinkPtr prev = tail;

        do
        {
            if (cmp(l->data, key) == 0)
            {
                prev->next = l->next;  /* unlink */
                *remlink = l;
                if (l == tail)
                {                      /* check for last item removal */
                    return (tail->next == tail ? NULL : prev);
                }
                else
                {
                    return tail;       /* success */
                }
            }
            prev = l;
            l = l->next;
        } while (l != head);

        *remlink = NULL;
    }
    return tail;                       /* failure */
}

/*
 * clink_find() --Find a particular item in a linked-list.
 *
 * Parameters:
 * tail --tail of the (circular) list
 * cmp  --the comparison function used to match the item
 * key  --the key value used to compare items
 *
 * Returns: (void *)
 * Success: the item; Failure: NULL.
 */
void *clink_find(LinkPtr tail, CompareProc cmp, void *key)
{
    LinkPtr head;
    LinkPtr l;

    if (tail != NULL)
    {
        head = tail->next;
        l = head;
        do
        {
            if (cmp(l->data, key) == 0)
            {
                return (l->data);      /* success */
            }
            l = l->next;
        } while (l != head);
    }
    return NULL;                       /* failure */
}

/*
 * clink_visit() --Visit all the items in a linked-list.
 *
 * Parameters:
 * tail --tail of the (circular) list
 * visit    --the visit function to call on every item
 * usrdata  --"usrdata" data to call visit() with.
 *
 * Returns: (void *)
 * The item "selected" by visit(), or NULL.
 *
 * Remarks:
 * clink_visit() will call visit() with every item on the list
 * until visit() returns non-null; this value is passed
 * back to the caller.
 *
 * See Also: clink_find, clink_insert
 */
void *clink_visit(LinkPtr tail, VisitProc visit, void *usrdata)
{
    LinkPtr head = tail->next;
    LinkPtr l = head;

    do
    {
        if (visit(l->data, usrdata) != 0)
        {
            return (l->data);          /* success */
        }
        l = l->next;
    } while (l != head);
    return NULL;                       /* failure */
}
