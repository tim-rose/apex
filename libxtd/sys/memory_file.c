/*
 * MEMORY_FILE.C --Routines for implementing FILE access to in-memory text.
 *
 * Contents:
 * mem_read()  --Read bytes from a memory-file.
 * mem_write() --Write bytes to a memory-file.
 * mem_seek()  --Seek to a position in a memory-file.
 * mem_close() --Close/reset a memory-file.
 * mem_open()  --Open a stdio file pointer on a chunk of memory.
 *
 * Remarks:
 * This module provides the necessary routines for manipulating a
 * chunk of text via a FILE pointer.  It uses BSD's funopen(3).
 * It is similar to fmemopen(3), but mem_open() avoids malloc(3).
 */
#include <errno.h>
#include <string.h>
#include <xtd.h>
#include <memory_file.h>

static const MemoryFile empty_mem_file = { 0 };

/*
 * mem_read() --Read bytes from a memory-file.
 */
static int mem_read(void *cookie, char *bytes, int size)
{
    MemoryFilePtr file = (MemoryFilePtr) cookie;
    int n_bytes = MIN(file->size - file->position, size);

    if (n_bytes > 0)
    {
        memcpy(bytes, file->data + file->position, n_bytes);
        file->position += n_bytes;
    }
    return n_bytes;
}

/*
 * mem_write() --Write bytes to a memory-file.
 */
static int mem_write(void *cookie, const char *bytes, int size)
{
    MemoryFilePtr file = (MemoryFilePtr) cookie;
    int n_bytes = MIN(file->size - file->position, size);

    if (n_bytes > 0)
    {
        memcpy(file->data + file->position, bytes, n_bytes);
        file->position += n_bytes;
    }
    return n_bytes;
}

/*
 * mem_seek() --Seek to a position in a memory-file.
 */
static fpos_t mem_seek(void *cookie, fpos_t position, int whence)
{
    MemoryFilePtr file = (MemoryFilePtr) cookie;
    fpos_t new_position = 0;

    switch (whence)
    {
    case SEEK_SET:
        new_position = position;
        break;
    case SEEK_CUR:
        new_position += file->position + position;
        break;
    case SEEK_END:
        new_position = file->size + position;
        break;
    }
    if (new_position < 0 || new_position >= file->size)
    {
        errno = EINVAL;
        return -1;                     /* error: position outside range. */
    }
    file->position = new_position;
    return 0;
}

/*
 * mem_close() --Close/reset a memory-file.
 */
static int mem_close(void *cookie)
{
    if (cookie != NULL)
    {
        MemoryFilePtr file = (MemoryFilePtr) cookie;
        *file = empty_mem_file;        /* reset data structure for safety */
        return 0;
    }
    return -1;
}

/*
 * mem_open() --Open a stdio file pointer on a chunk of memory.
 *
 * Parameters:
 * file --an unitialised memory file pointer
 * bytes --the address of the memory to read/write as a FILE
 * size --the size of the memory chunk
 *
 * Returns: (FILE *)
 * Success: the FILE pointer; Failure: NULL.
 *
 * Remarks:
 * For now, this returns a FILE that can read and write.
 * REVISIT: consider adding support for file mode arg.
 */
FILE *mem_open(MemoryFilePtr file, char *bytes, int size)
{
    *file = empty_mem_file;
    file->size = size;
    file->data = bytes;
    return funopen((void *) file, mem_read, mem_write, mem_seek, mem_close);
}
