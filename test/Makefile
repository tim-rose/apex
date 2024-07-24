#
# Makefile --Build rules for the test directory
#
export DYLD_LIBRARY_PATH=../libapex/$(archdir)
language 	= c
BUILD_PATH = ../libapex

C_SRC = test-apex.c test-binsearch.c test-convert.c test-csv.c \
    test-date.c test-estring.c test-getopts.c test-hash.c \
    test-heap-sift.c test-heap.c test-log-parse.c \
    test-log.c test-nmea.c test-pool.c test-protocol.c \
    test-queue.c test-stack.c test-stately-failure.c \
    test-stately-turnstile.c test-symbol.c test-systools.c \
    test-tfile.c test-url.c test-vector.c
C_MAIN_SRC = test-apex.c test-binsearch.c test-convert.c test-csv.c \
    test-date.c test-estring.c test-getopts.c test-hash.c \
    test-heap-sift.c test-heap.c test-log-parse.c \
    test-log.c test-nmea.c test-pool.c test-protocol.c \
    test-queue.c test-stack.c test-stately-failure.c \
    test-stately-turnstile.c test-symbol.c test-systools.c \
    test-tfile.c test-url.c test-vector.c

include makeshift.mk test/tap.mk

$(C_MAIN):	-lapex

test-tap: $(C_MAIN)
