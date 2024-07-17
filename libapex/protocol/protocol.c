/*
 * PROTOCOL.C --Various utilities for implementing protocols.
 *
 * Contents:
 * open_connect()   --Open a socket, and "connect" to it.
 * open_listen()    --Open a socket, bind and "listen" to it.
 * fdread()         --Read some bytes from a descriptor.
 * fdwrite()        --Write some bytes to a descriptor.
 * pack()           --Pack a value into a memory buffer.
 * unpack()         --Unpack a value from a memory buffer.
 * _setup_address() --setup the sockaddr structure for a socket.
 *
 * Remarks:
 * Low-level protocol functions typically need to do system calls
 * and bash bytes in/out buffers.  This module provides some convenience
 * functions for that purpose.
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <limits.h>
#ifdef __WINNT__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif /* __WINNT__ */
#include <apex.h>
#include <apex/protocol.h>

static size_t _setup_address(const char *address, int domain,
                             int type, struct sockaddr_storage *addr);

/*
 * open_connect() --Open a socket, and "connect" to it.
 *
 * Parameters:
 * address  --the socket (address-family dependent) address
 * domain   --the protocol domain/address family (e.g. PF_INET, PF_LOCAL)
 * type     --the socket type
 *
 * Returns: (int)
 * Success: the socket descriptor; Failure: -1.
 *
 * Remarks:
 * This is a convenience routine for client code.  In particular, the
 * "address" is intended to contain all that's required to setup the
 * connection, which for AF_INET domain is the IP address and port w/
 * the usual syntax "hostname:port".  For AF_LOCAL domains, the
 * address is the filesystem pathname.
 */
int open_connect(const char *address, int domain, int type)
{
    int sfd;
    struct sockaddr_storage addr_buf = {.ss_family = 0 };
    size_t len;

    sfd = -1;
    len = _setup_address(address, domain, type, &addr_buf);

    if (len != 0 && (sfd = socket(domain, type, 0)) >= 0)
    {
        if (connect(sfd, (struct sockaddr *) &addr_buf, len) < 0)
        {
            close(sfd);
            sfd = -1;                  /* error: connect failed */
        }
    }
    return sfd;
}

/*
 * open_listen() --Open a socket, bind and "listen" to it.
 *
 * Parameters:
 * address  --the socket (address-family dependent) address
 * domain   --the protocol domain/address family
 * type     --the socket type
 *
 * Returns: (int)
 * Success: the socket descriptor; Failure: -1.
 *
 * Remarks:
 * This is a convenience routine for server code.
 */
int open_listen(const char *address, int domain, int type)
{
    int sfd;
    struct sockaddr_storage addr_buf = {.ss_family = 0 };
    size_t len;

    sfd = -1;
    len = _setup_address(address, domain, type, &addr_buf);

    if (len != 0 && (sfd = socket(domain, type, 0)) >= 0)
    {
        if (bind(sfd, (struct sockaddr *) &addr_buf, len) < 0
            || listen(sfd, 1) < 0)
        {
            close(sfd);
            sfd = -1;                  /* error: bind/listen failed */
        }
    }
    return sfd;
}

/*
 * fdread() --Read some bytes from a descriptor.
 *
 * Parameters:
 * fd   --the descriptor to read from
 * vptr --the destination buffer to place the read bytes
 * n    --the number of bytes to read
 *
 *  Returns: (ssize_t)
 *  Success: the number of bytes read; Failure: -1.
 *
 *  Remarks:
 *  This routine provides a primitive buffering/retry layer over
 *  raw read() calls.
 *
 *  fdread() has been adapted from: Unix Network Programming (Stevens)
 */
ssize_t fdread(int fd, void *vptr, size_t n)
{
    char *ptr = vptr;
    size_t nleft = n;

    while (nleft > 0)
    {
        ssize_t nr;

        if ((nr = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                nr = 0;                /* retry */
            }
            else
            {
                return -1;             /* error */
            }
        }
        else if (nr == 0)
        {
            break;                     /* EOF */
        }
        nleft -= (size_t) nr;
        ptr += nr;
    }
    return (ssize_t) (n - nleft);
}

/*
 * fdwrite() --Write some bytes to a descriptor.
 *
 * Parameters:
 * fd   --the descriptor to write from
 * vptr --the source buffer of bytes to be written
 * n    --the number of bytes to write
 *
 *  Returns: (ssize_t)
 *  Success: the number of bytes written; Failure: -1.
 *
 *  Remarks:
 *  fdwrite() has been adapted from: Unix Network Programming (Stevens)
 */
ssize_t fdwrite(int fd, const void *vptr, size_t n)
{
    const char *ptr = vptr;
    size_t nleft = n;

    while (nleft > 0)
    {
        ssize_t nw;

        if ((nw = write(fd, ptr, nleft)) <= 0)
        {
            if (nw < 0 && errno == EINTR)
            {
                nw = 0;                /* retry */
            }
            else
            {
                return -1;             /* error */
            }
        }
        nleft -= (size_t) nw;
        ptr += nw;
    }
    return (ssize_t) n;
}

/*
 * pack() --Pack a value into a memory buffer.
 *
 * Parameters:
 * fmt  --a character that controls the packing
 * mem  --the memory buffer to pack the data into
 * item --a pointer to the data to be packed
 *
 * Returns: (size_t)
 * The number of bytes written to mem.
 *
 * Remarks:
 * This API was inspired by the perl functions of similar name.
 * The format string is a simple list of character codes that
 * define how the various items are packed:
 * * c  --signed char
 * * C  --unsigned char
 * * s  --signed short (16 bits)
 * * S  --unsigned short
 * * l  --signed long (32 bits)
 * * L  --unsigned long
 * * n  --unsigned short in network byte-order
 * * N  --unsigned long in network byte-order
 * * Z  --simple "C" (i.e. NUL-terminated) string
 *
 * This routine (and its counterpart, unpack()), take care to avoid
 * misaligned memory accesses, so it can be used to create a packed
 * byte stream.
 */
size_t pack(int fmt, uint8_t * ptr, void *item)
{
    uint16_t uint_16;
    uint32_t uint_32;

    switch (fmt)
    {
    case 'c':                         /* signed,unsigned char */
    case 'C':
        *(uint8_t *) ptr = *(uint8_t *) item;
        return sizeof(uint8_t);
    case 's':                         /* signed,unsigned short */
    case 'S':
        memcpy(ptr, item, sizeof(uint16_t));
        return sizeof(uint16_t);
    case 'l':                         /* signed,unsigned long */
    case 'L':
        memcpy(ptr, item, sizeof(uint32_t));
        return sizeof(uint32_t);
    case 'n':                         /* unsigned short network byte-order */
        uint_16 = htons(*(uint16_t *) item);
        memcpy(ptr, &uint_16, sizeof uint_16);
        return sizeof(uint16_t);
    case 'N':                         /* unsigned long, network byte-order */
        uint_32 = htonl(*(uint32_t *) item);
        memcpy(ptr, &uint_32, sizeof uint_32);
        return sizeof(uint32_t);
    case 'Z':                         /* "C" NUL-terminated string */
        strcpy((char *) ptr, item);    /* UNSAFE */
        return strlen(item) + 1;
    default:
        break;
    }
    return 0;
}

/*
 * unpack() --Unpack a value from a memory buffer.
 *
 * Parameters:
 * fmt  --a character that controls the unpacking
 * mem  --the memory buffer to unpack the data from
 * item -- a pointer to the destination value
 *
 * Returns: (size_t)
 * The number of bytes read (i.e. consumed) from mem.
 */
size_t unpack(int fmt, uint8_t * ptr, void *item)
{
    uint16_t uint_16;
    uint32_t uint_32;

    switch (fmt)
    {
    case 'c':                         /* signed, unsigned char */
    case 'C':
        *(uint8_t *) item = *(uint8_t *) ptr;
        return sizeof(uint8_t);
    case 's':                         /* signed, unsigned short */
    case 'S':
        memcpy(item, ptr, sizeof(uint16_t));
        return sizeof(uint16_t);
    case 'l':                         /* signed, unsigned long */
    case 'L':
        memcpy(item, ptr, sizeof(uint32_t));
        return sizeof(uint32_t);
    case 'n':                         /* unsigned short, network byte-order */
        memcpy(&uint_16, ptr, sizeof uint_16);
        *(uint16_t *) item = ntohs(uint_16);
        return sizeof(uint16_t);
    case 'N':                         /* unsigned long, network byte-order */
        memcpy(&uint_32, ptr, sizeof uint_32);
        *(uint32_t *) item = ntohl(uint_32);
        return sizeof(uint32_t);
    case 'Z':                         /* "C" NUL-terminated string */
        strcpy(item, (char *) ptr);
        return strlen((char *) ptr) + 1;
    default:
        break;
    }
    return 0;
}

/*
 * _setup_address() --setup the sockaddr structure for a socket.
 *
 * Parameters:
 * address  --the socket (address-family dependent) address
 * domain   --the protocol domain/address family
 * type     --the socket type
 * addr     --a caller-supplied sockaddr to be initialised
 *
 * Returns: (int)
 * Success: the size of the "true" sockaddr; Failure: 0.
 *
 * Remarks:
 * This routine encapsulates the protocol-specific processing
 * for initialising the sockaddr structure.
 *
 * TODO: add AF_INET6 support.
 */
static size_t _setup_address(const char *address, int domain,
                             int type, struct sockaddr_storage *addr)
{
    char host[FILENAME_MAX];
    char service[FILENAME_MAX];
    size_t addr_len;

    struct addrinfo *info;
    struct addrinfo hint = {.ai_family = 0 };

    if (sscanf(address, "%[^:]:%s", host, service) != 2)
    {
        return 0;                      /* error: can't parse "host:port" */
    }
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = domain;
    hint.ai_socktype = type;

    if (getaddrinfo(host, service, &hint, &info) == 0)
    {
        addr_len = info->ai_addrlen;
        memcpy(addr, info->ai_addr, addr_len);
        freeaddrinfo(info);
        return addr_len;
    }
    return 0;                          /* error: unsupported family? */
}
