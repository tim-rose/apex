#
# Makefile --Build rules for the test directory
#
export DYLD_LIBRARY_PATH=../libxtd/$(archdir)
language 	= c
BUILD_PATH = ../libxtd

C_SRC = test-binsearch.c test-convert.c test-csv.c \
    test-date.c test-estring.c test-getopts.c test-hash.c \
    test-heap-template.c test-heap.c test-log-parse.c \
    test-log.c test-nmea.c test-protocol.c test-stack.c \
    test-symbol.c test-systools.c test-tfile.c test-url.c \
    test-vector.c test-xtd.c
C_MAIN_SRC = test-binsearch.c test-convert.c test-csv.c \
    test-date.c test-estring.c test-getopts.c test-hash.c \
    test-heap-template.c test-heap.c test-log-parse.c \
    test-log.c test-nmea.c test-protocol.c test-stack.c \
    test-symbol.c test-systools.c test-tfile.c test-url.c \
    test-vector.c test-xtd.c

include makeshift.mk test/tap.mk

$(C_MAIN):	-lxtd

test-tap: $(C_MAIN)
