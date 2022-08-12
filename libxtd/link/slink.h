/*
 * SLINK.H --Definitions for singly-linked lists.
 *
 */
#ifndef LINK_H
#define LINK_H

#include <xtd/binsearch.h>                 /* for CompareProc */

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
    } LinkInsertMode, *LinkInsertModePtr;

    /*
     * VisitProc --Visitor function for LinkVisit().
     */
    typedef void *(*VisitProc)(void *data, void *closure);

    /*
     * Link --Singly-linked list node structure.
     */
    typedef struct Link_t
    {
        struct Link_t *next;           /* next item in list (CAR!) */
        void *data;                    /* user-data (CDR!) */

    } Link, *LinkPtr;

    LinkPtr link_new(LinkPtr next, void *data); /* (CONS!) */
    void link_free(LinkPtr link);
    void link_free_links(LinkPtr head, LinkPtr tail);
#ifdef __cplusplus
}
#endif                                 /* C++ */

#endif                                 /* LINK_H */
