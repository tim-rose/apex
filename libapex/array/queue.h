/*
 * QUEUE.H --A simple atomic queue.
 *
 * Contents:
 * AtomicQueue_t{} --The state of a queue and its working storage.
 *
 * Remarks:
 * This queue is thread safe for single-producer, single-consumer.
 */
#ifndef QUEUE_H
#define QUEUE_H
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
    typedef struct AtomicQueue_t
    {
        ArrayContainer array;          /* Queue contents. */

        int n_read;                    /* No. of successful reads */
        int n_write;                   /* No. of successful writes  */
        int n_fail;                    /* No. of failed writes  */
        int mask;                      /* True size of working storage, as a mask. */
    } AtomicQueue, *AtomicQueuePtr;

    int queue_mask(int n, int *mask);
    AtomicQueuePtr queue_alloc(void);
    AtomicQueuePtr queue_init(AtomicQueuePtr queue, int n_items,
                              int item_size, void *items);

    int queue_push(AtomicQueuePtr queue, const void *item);
    int queue_pop(AtomicQueuePtr queue, void *item);
    void *queue_peek(AtomicQueuePtr queue, void *item);

    /*
     * Convenience functions for malloc and item-size aware initialisation.
     */
#define new_queue(items) init_queue(queue_alloc(), items)
#define init_queue(queue, items) queue_init(queue, NEL(items), sizeof(items[0]), items)

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* QUEUE_H */
