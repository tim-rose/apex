#
# Makefile --Build rules for "apex", my extensions to the C library.
#
language = c
LIB_ROOT = ..
subdir = apex

C_SRC = http.c protocol.c url.c
H_SRC = http.h protocol.h url.h


include makeshift.mk library.mk

$(archdir)/protocol.o: TARGET.CFLAGS = -Wno-sign-conversion

install: install-lib-include
uninstall: uninstall-lib-include
