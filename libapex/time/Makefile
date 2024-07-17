#
language = c
LIB_ROOT = ..
subdir = apex

C_SRC = adjust.c date.c strptime.c timeval.c
H_SRC = date.h timeval.h


include makeshift.mk library.mk

$(archdir)/date.o: TARGET.CFLAGS = -Wno-format-nonliteral
$(archdir)/timeval.o: TARGET.CFLAGS = -Wno-format-nonliteral

install: install-lib-include
uninstall: uninstall-lib-include
