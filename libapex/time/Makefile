#
# Makefile --Build rules for "apex", my extensions to the C library.
#
language = c
LIB_ROOT = ..
subdir = apex

C_SRC = adjust.c date.c timeval.c
H_SRC = date.h timeval.h

include makeshift.mk library.mk
install: install-lib-include
uninstall: uninstall-lib-include
