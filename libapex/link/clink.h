/*
 * LINK.H --Definitions for singly-linked lists.
 *
 */
#ifndef CLINK_H
#define CLINK_H

#include <apex/slink.h>                 /* for Link, CompareProc, VisitProc */

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    int clink_len(LinkPtr tail);
    LinkPtr clink_reverse(LinkPtr tail);
    LinkPtr clink_rotate(LinkPtr tail, int n);
    LinkPtr clink_add(LinkPtr tail, LinkPtr link);

    LinkPtr clink_insert(LinkPtr tail, CompareProc cmp, void *value,
                         LinkInsertMode mode);
    /* REVISIT: clink_merge() not implemented yet */
    LinkPtr clink_merge(LinkPtr l1, LinkPtr l2, LinkInsertMode mode);
    LinkPtr clink_remove(LinkPtr tail, CompareProc cmp, void *key,
                         LinkPtr * rlink);
    void *clink_find(LinkPtr tail, CompareProc cmp, void *key);
    void *clink_visit(LinkPtr tail, VisitProc visit, void *usrdata);

    /*
     * clink_append() --Add a single link to the tail of an existing linked list.
     *
     * Parameters:
     * tail --the tail of the list to add this link
     * link --the link to add.
     *
     * Returns: (LinkPtr)
     * the tail of the modified list.
     */
#define clink_append(tail, link) clink_add((tail), (link))->next
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* CLINK_H */
