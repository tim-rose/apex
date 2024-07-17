/*
 * QUEUE.C --An atomic non-blocking queue of items.
 *
 * Contents:
 * queue_alloc() --Allocate some space for a queue structure.
 * queue_mask()  --Calculate/validate the mask for a specified size.
 * queue_init()  --Initialise a queue with the specified working storage.
 * queue_push()  --Push an item onto the queue.
 * queue_pop()   --Pop an item from the queue.
 * queue_peek()  --Read, but do not remove, an item from the queue.
 */
#include <errno.h>
#include <string.h>
#include <apex/queue.h>

/*
 * queue_alloc() --Allocate some space for a queue structure.
 *
 * Returns: (QueuePtr)
 * Success: the allocated memory; Failure: NULL.
 */
/* LCOV_EXCL_START */
AtomicQueuePtr queue_alloc(void)
{
    return malloc(sizeof(AtomicQueue));
}

/* LCOV_EXCL_STOP */

/*
 * queue_mask() --Calculate/validate the mask for a specified size.
 *
 * Parameter:
 * n    --the queue size (should be a power of 2)
 *
 * Returns:
 * Success: 1; Failure: 0..
 *
 * Remarks:
 * This routine fails if the size is not a power of 2, using
 * non-obvious(?) bit wrangling to test that.  The mask for a (power
 * of 2) n is all bits set from that power's bit down, or (now
 * shifting to further non-obvious arithmetic/bit wrangling, is n - 1.
 */
int queue_mask(size_t n, size_t *mask)
{
    if ((n & (n - 1)) != 0)
    {
        return 0;                      /* failure: not a simple power of 2 */
    }
    *mask = n - 1;
    return 1;                          /* success: mask has been set */
}

/*
 * queue_init() --Initialise a queue with the specified working storage.
 *
 * Parameters:
 * queue    --The queue to be initialised (owned by caller).
 * n_items  --The number of items in the queue (must power of 2)
 * item_size    --The size of the queue items.
 * base    --The working storage of the queue.
 *
 * Returns:
 * queue.
 *
 * Remarks:
 * This function initialises the provided `queue` structure, and
 * attaches it to the working storage.  Note that the queue
 * implementation requires that the size of the queue is a simple
 * power of 2.
 */
AtomicQueuePtr queue_init(AtomicQueuePtr queue, size_t n_items,
                          size_t item_size, void *base)
{
    if (queue == NULL || base == NULL || n_items == 0)
    {
        return NULL;                   /* failure: no queue, no items? */
    }
    memset((void *) queue, 0, sizeof(*queue));
    if (!queue_mask(n_items, &queue->mask))
    {                                  /* REVISIT: assert? */
        return NULL;                   /* error: bad size */
    }
    array_init(&queue->array, n_items, item_size, base);
    return queue;                      /* success: queue is initialised */
}

/*
 * queue_push() --Push an item onto the queue.
 *
 * queue --the queue to receive the item
 * item --the queue item to insert
 *
 * @return Success: 1; Failure: 0.
 *
 * If the queue is full, this function will fail (and update the
 * failure count).
 */
int queue_push(AtomicQueuePtr queue, const void *item)
{
    if (queue == NULL || item == NULL)
    {
        return 0;                      /* failure: no queue, or no item! */
    }
    if (queue->n_write - queue->n_read > queue->mask)
    {
        queue->n_fail++;               /* remember failures */
        return 0;                      /* failure: queue is full */
    }
    memcpy(queue->array.base
           + queue->array.item_size * (queue->n_write & queue->mask),
           item, queue->array.item_size);
    queue->n_write++;
    return 1;                          /* success */
}

/*
 * queue_pop() --Pop an item from the queue.
 *
 * Parameters:
 * queue --the queue to read from
 * item --returns the item
 *
 * Returns:
 * Success: 1; Failure: 0.
 *
 * This function fails if the queue is empty.
 */
int queue_pop(AtomicQueuePtr queue, void *item)
{
    if (queue_peek(queue, item))
    {
        queue->n_read++;
        return 1;                      /* success: item copied */
    }
    return 0;                          /* failure: empty queue */
}

/*
 * queue_peek() --Read, but do not remove, an item from the queue.
 *
 * Parameter:
 * queue --the queue to read from
 * item --NULL, or returns the item
 *
 * Returns:
 * Success: a pointer to the item; Failure: NULL.
 *
 * This function fails if the queue is empty.
 */
void *queue_peek(AtomicQueuePtr queue, void *item)
{
    void *queue_item = NULL;

    if (queue != NULL)
    {
        if (queue->n_write != queue->n_read)
        {
            queue_item = queue->array.base
                + queue->array.item_size * (queue->n_read & queue->mask);
            if (item != NULL)
            {
                memcpy(item, queue_item, queue->array.item_size);
            }
        }
    }
    return queue_item;                 /* success: item, failure: NULL */
}
