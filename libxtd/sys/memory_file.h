/*
 * MEMORY_FILE.H --Open an in-memory string as a stdio file.
 *
 * Remarks:
 * This uses funopen(3) to create a FILE that reads/writes to
 * an in-memory chunk of text.
 */
#ifndef MEMORY_FILE_H
#define MEMORY_FILE_H
#include <stdio.h>

/*
 * MemoryFile --Structure for managing the I/O state of a memory-file.
 */
typedef struct MemoryFile_t
{
    fpos_t position;                    /* current position in data[] */
    fpos_t size;                        /* size of data[] in bytes */
    char *data;                         /* pointer to data[] */
} MemoryFile, *MemoryFilePtr;
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    FILE *mem_open(MemoryFilePtr file, char *bytes, int size);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* MEMORY_FILE_H */
