/*
 * SLINK.H --Definitions for singly-linked lists.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 */
#ifndef APEX_LINK_H
#define APEX_LINK_H

#include <apex/search.h>             /* for CompareProc */

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */

    /*
     * LinkInsertMode --Control the behaviour of Link insert/duplicate handling.
     */
    typedef enum
    {
        LINK_INSERT_FAIL = 0,
        LINK_INSERT_REPLACE,
        LINK_INSERT_DUPLICATE
    } LinkInsertMode;

    /*
     * VisitProc --Visitor function for LinkVisit().
     */
    typedef int (*VisitProc)(void *data, void *closure);

    /*
     * Link --Singly-linked list node structure.
     */
    typedef struct Link
    {
        struct Link *next;           /* next item in list (CAR!) */
        void *data;                    /* user-data (CDR!) */

    } Link;

    Link *link_new(Link *next, void *data); /* (CONS!) */
    void link_free(Link *link);
    void link_free_links(Link *head, Link *tail);
#ifdef __cplusplus
}
#endif                                 /* C++ */

#endif                                 /* APEX_LINK_H */
