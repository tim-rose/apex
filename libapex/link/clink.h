/*
 * LINK.H --Definitions for singly-linked lists.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 */
#ifndef APEX_CLINK_H
#define APEX_CLINK_H

#include <apex/slink.h>                 /* for Link, CompareProc, VisitProc */

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    int clink_len(Link *tail);
    Link *clink_reverse(Link *tail);
    Link *clink_rotate(Link *tail, int n);
    Link *clink_add(Link *tail, Link *link);

    Link *clink_insert(Link *tail, CompareProc cmp, void *value,
                         LinkInsertMode mode);
    /* REVISIT: clink_merge() not implemented yet */
    Link *clink_merge(Link *l1, Link *l2, LinkInsertMode mode);
    Link *clink_remove(Link *tail, CompareProc cmp, void *key,
                         Link ** rlink);
    void *clink_find(Link *tail, CompareProc cmp, void *key);
    void *clink_visit(Link *tail, VisitProc visit, void *usrdata);

    /*
     * clink_append() --Add a single link to the tail of an existing linked list.
     *
     * Parameters:
     * tail --the tail of the list to add this link
     * link --the link to add.
     *
     * Returns: (Link *)
     * the tail of the modified list.
     */
#define clink_append(tail, link) clink_add((tail), (link))->next
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_CLINK_H */
