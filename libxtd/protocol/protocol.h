/*
 * PROTOCOL.H --Various utilities for implementing protocols.
 *
 * Remarks:
 * This module contains some routines for handling sockets and
 * manipulating packets.  In particular, pack(), unpack() are
 * inspired by their Perl namesakes.
 *
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    int open_connect(const char *address, int domain, int type);
    int open_listen(const char *address, int domain, int type);
    ssize_t readn(int fd, void *vptr, size_t n);
    ssize_t writen(int fd, void *vptr, size_t n);
    size_t pack(int fmt, uint8_t * ptr, void *item);
    size_t unpack(int fmt, uint8_t * ptr, void *item);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* PROTOCOL_H */
