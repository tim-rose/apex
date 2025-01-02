/*
 * QUEUE.H --A simple atomic queue.
 *
 * Contents:
 * AtomicQueue_t{} --The state of a queue and its working storage.
 *
 * Remarks:
 * This queue is thread safe for single-producer, single-consumer.
 */
#ifndef APEX_QUEUE_H
#define APEX_QUEUE_H
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
#include <stddef.h>
#include <apex/array.h>
    /*
     * AtomicQueue_t{} --The state of a queue and its working storage.
     *
     * Remarks:
     * The queue counts the No. of reads and writes as simple
     * integers, allowing them to simply overflow.  This strategy
     * works if the size of the queue is a power of 2
     * (guaranteed by `queue_init`), and it relies on "natural"
     * int arithmetic to be atomic for its thread safety.
     */
    typedef struct AtomicQueue
    {
        ArrayContainer array;          /* Queue contents. */

        size_t n_read;                    /* No. of successful reads */
        size_t n_write;                   /* No. of successful writes  */
        size_t n_fail;                    /* No. of failed writes  */
        size_t mask;                      /* True size of working storage, as a mask. */
    } AtomicQueue;

    int queue_mask(size_t n, size_t *mask);
    AtomicQueue *queue_alloc(void);
    AtomicQueue *queue_init(AtomicQueue *queue, size_t n_items,
                              size_t item_size, void *items);

    int queue_push(AtomicQueue *queue, const void *item);
    int queue_pop(AtomicQueue *queue, void *item);
    void *queue_peek(AtomicQueue *queue, void *item);

    /*
     * Convenience functions for malloc and item-size aware initialisation.
     */
#define new_queue(items) init_queue(queue_alloc(), items)
#define init_queue(queue, items) queue_init(queue, NEL(items), sizeof(items[0]), items)

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_QUEUE_H */
