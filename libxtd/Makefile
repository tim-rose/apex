#
# Makefile --Build rules for "xtd", some extensions to the C library.
#
#LIB_TYPE = static shared
language = c
H_SRC = xtd.h

SUBLIB_SRC = array/$(archdir)/lib.a config/$(archdir)/lib.a \
    csv/$(archdir)/lib.a hash/$(archdir)/lib.a \
    link/$(archdir)/lib.a log/$(archdir)/lib.a \
    parse/$(archdir)/lib.a protocol/$(archdir)/lib.a \
    stately/$(archdir)/lib.a string/$(archdir)/lib.a \
    symbol/$(archdir)/lib.a sys/$(archdir)/lib.a \
    tap/$(archdir)/lib.a tfile/$(archdir)/lib.a \
    time/$(archdir)/lib.a timer/$(archdir)/lib.a \
    vector/$(archdir)/lib.a xtd/$(archdir)/lib.a

MK_SRC = target-depend.mk

include makeshift.mk library.mk
include target-depend.mk

install: install-lib
uninstall: uninstall-lib
