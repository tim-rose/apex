#
# Makefile --Build rules for "xtd", some extensions to the C library.
#
#LIB_TYPE = static shared
language = c nroff

SUBLIB_SRC = config/$(archdir)/lib.a csv/$(archdir)/lib.a \
    hash/$(archdir)/lib.a heap/$(archdir)/lib.a \
    link/$(archdir)/lib.a log/$(archdir)/lib.a \
    misc/$(archdir)/lib.a parse/$(archdir)/lib.a \
    protocol/$(archdir)/lib.a string/$(archdir)/lib.a \
    symbol/$(archdir)/lib.a sys/$(archdir)/lib.a \
    tap/$(archdir)/lib.a tfile/$(archdir)/lib.a \
    time/$(archdir)/lib.a vector/$(archdir)/lib.a
H_SRC = gnuattr.h xtd.h

MK_SRC = target-depend.mk

include devkit.mk library.mk
include target-depend.mk

install: install-lib
uninstall: uninstall-lib
