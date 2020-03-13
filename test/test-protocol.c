/*
 * PROTOCOL.C --Unit tests for the "protocol" functions.
 *
 * Contents:
 * big_endian()  --Test if this program is running on a big-endian machine.
 * test_pack()   --Test the behaviour of pack().
 * test_unpack() --Test the behaviour of unpack().
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <tap.h>
#include <protocol.h>

static int big_endian(void);
static void test_pack(void);
static void test_unpack(void);

int main(void)
{
    plan_tests(32);

    test_pack();
    test_unpack();

    return exit_status();
}

/*
 * big_endian() --Test if this program is running on a big-endian machine.
 */
static int big_endian(void)
{
    int one = 1;

    return !(*((char *) (&one)));
}

/*
 * test_pack() --Test the behaviour of pack().
 */
static void test_pack(void)
{
    uint8_t buf[100];
    unsigned char c = 0xa5;
    size_t n;
    uint16_t uint16_src = 0xbeef, uint16_nbo;
    uint32_t uint32_src = 0xdeadc0de, uint32_nbo;

    uint16_nbo = (uint16_t) big_endian()? 0xbeef : 0xefbe;
    uint32_nbo = (uint32_t) big_endian()? 0xdeadc0de : 0xdec0adde;

    /*
     * character tests
     */
    n = pack('C', buf, &c);
    ok(n == 1, "pack(C): size");
    ok((unsigned char) buf[0] == c, "pack(C): value");

    n = pack('c', buf, &c);
    ok(n == 1 && (unsigned char) buf[0] == c, "pack(c): signed value");

    /*
     * short tests
     */
    n = pack('S', buf, &uint16_src);
    ok(n == 2, "pack(s): size");
    ok(memcmp(buf, &uint16_src, 2) == 0, "pack(S): value");

    n = pack('s', buf, &uint16_src);
    ok(n == 2 && memcmp(buf, &uint16_src, 2) == 0, "pack(s): signed value");

    n = pack('s', buf + 1, &uint16_src);
    ok(n == 2 && memcmp(buf + 1, &uint16_src, 2) == 0, "pack(s): alignment");

    n = pack('n', buf, &uint16_src);
    ok(n == 2 &&
       memcmp(buf, &uint16_nbo, 2) == 0, "pack(n): network byte order");

    /*
     * long tests
     */
    n = pack('L', buf, &uint32_src);
    ok(n == 4, "pack(s): size");
    ok(memcmp(buf, &uint32_src, 4) == 0, "pack(L): value");

    n = pack('l', buf, &uint32_src);
    ok(n == 4 && memcmp(buf, &uint32_src, 4) == 0, "pack(l): signed value");

    n = pack('l', buf + 1, &uint32_src);
    ok(n == 4 &&
       memcmp(buf + 1, &uint32_src, 4) == 0, "pack(l): alignment 1");
    n = pack('l', buf + 2, &uint32_src);
    ok(n == 4 &&
       memcmp(buf + 2, &uint32_src, 4) == 0, "pack(l): alignment 2");
    n = pack('l', buf + 3, &uint32_src);
    ok(n == 4 &&
       memcmp(buf + 3, &uint32_src, 4) == 0, "pack(l): alignment 3");

    n = pack('N', buf, &uint32_src);
    ok(n == 4 &&
       memcmp(buf, &uint32_nbo, 4) == 0, "pack(n): network byte order");

    /*
     * string tests
     */
    n = pack('Z', buf, (void *) "hello");
    ok(n == 6 && strcmp((char *) buf, "hello") == 0, "pack(Z): string");
}

/*
 * test_unpack() --Test the behaviour of unpack().
 */
static void test_unpack(void)
{
    uint8_t buf[100];
    unsigned char c = 0xa5;
    size_t n;
    uint16_t uint16_src = 0xbeef, uint16_nbo, uint16_dst;
    uint32_t uint32_src = 0xdeadc0de, uint32_nbo, uint32_dst;

    uint16_nbo = (uint16_t) big_endian()? 0xbeef : 0xefbe;
    uint32_nbo = (uint32_t) big_endian()? 0xdeadc0de : 0xdec0adde;

    /*
     * character tests
     */
    n = pack('C', buf, &c);
    n = unpack('C', buf, &c);
    ok(n == 1, "unpack(C): size");
    ok((unsigned char) buf[0] == 0xa5, "unpack(C): value");

    n = unpack('c', buf, &c);
    ok(n == 1 && (unsigned char) buf[0] == 0xa5, "unpack(c): signed value");

    /*
     * short tests
     */
    n = pack('S', buf, &uint16_src);
    n = unpack('S', buf, &uint16_dst);
    ok(n == 2, "unpack(s): size");
    ok(memcmp(&uint16_src, &uint16_dst, 2) == 0, "unpack(S): value");

    n = pack('s', buf, &uint16_src);
    n = unpack('s', buf, &uint16_dst);
    ok(n == 2 &&
       memcmp(&uint16_src, &uint16_dst, 2) == 0, "unpack(s): signed value");

    n = pack('s', buf + 1, &uint16_src);
    n = unpack('s', buf + 1, &uint16_src);
    ok(n == 2 &&
       memcmp(&uint16_src, &uint16_dst, 2) == 0, "unpack(s): alignment");

    n = pack('n', buf, &uint16_src);
    n = unpack('n', buf, &uint16_src);
    ok(n == 2 &&
       memcmp(&uint16_src, &uint16_dst, 2) == 0,
       "unpack(n): network byte order");
    /*
     * long tests
     */
    n = pack('L', buf, &uint32_src);
    n = unpack('L', buf, &uint32_dst);
    ok(n == 4, "unpack(s): size");
    ok(memcmp(&uint32_src, &uint32_dst, 4) == 0, "unpack(L): value");

    n = pack('l', buf, &uint32_src);
    n = unpack('l', buf, &uint32_dst);
    ok(n == 4 &&
       memcmp(&uint32_src, &uint32_dst, 4) == 0, "unpack(l): signed value");

    n = pack('l', buf + 1, &uint32_src);
    n = unpack('l', buf + 1, &uint32_dst);
    ok(n == 4 &&
       memcmp(&uint32_dst, &uint32_src, 4) == 0, "unpack(l): alignment 1");
    n = pack('l', buf + 2, &uint32_src);
    n = unpack('l', buf + 2, &uint32_dst);
    ok(n == 4 &&
       memcmp(&uint32_dst, &uint32_src, 4) == 0, "unpack(l): alignment 2");
    n = pack('l', buf + 3, &uint32_src);
    n = unpack('l', buf + 3, &uint32_dst);
    ok(n == 4 &&
       memcmp(&uint32_dst, &uint32_src, 4) == 0, "unpack(l): alignment 3");

    n = pack('N', buf, &uint32_src);
    n = unpack('N', buf, &uint32_dst);
    ok(n == 4 &&
       memcmp(&uint32_src, &uint32_dst, 4) == 0,
       "unpack(n): network byte order");

    /*
     * string tests
     */
    n = unpack('Z', (uint8_t *) "hello", buf);
    ok(n == 6 && strcmp((char *) buf, "hello") == 0, "unpack(Z): string");
}
