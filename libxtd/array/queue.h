/*
 * QUEUE.H --A simple atomic queue.
 */
#ifndef QUEUE_H
#define QUEUE_H
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
#include <stddef.h>
#include <array.h>
    /*
     * AtomicQueue_t{} --The state of a queue and its working storage.
     *
     * The queue counts the No. of reads and writes as simple
     * integers, allowing them to simply overflow.  This strategy
     * works if the size of the queue is a power of 2
     * (guaranteed by `queue_init`), and it relies on "natural"
     * int arithmetic to be atomic for its thread safety.
     */
    typedef struct AtomicQueue_t
    {
        ArrayContainer array;          /* Queue contents. */

        size_t n_read;                 /* No. of successful reads */
        size_t n_write;                /* No. of successful writes  */
        size_t n_fail;                 /* No. of failed writes  */
        size_t mask;                   /* True size of working storage, as a mask. */
    } AtomicQueue, *AtomicQueuePtr;

    int queue_mask(size_t n, size_t *mask);
    AtomicQueuePtr queue_alloc(void);
    AtomicQueuePtr queue_init(AtomicQueuePtr queue, size_t n_items,
                              size_t item_size, void *items);

    int queue_push(AtomicQueuePtr queue, const void *item);
    int queue_pop(AtomicQueuePtr queue, void *item);
    void *queue_peek(AtomicQueuePtr queue);

#define new_queue(items) init_queue(queue_alloc(), items)
#define init_queue(queue, items) queue_init(queue, NEL(items), sizeof(items[0]), items)

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* QUEUE_H */
