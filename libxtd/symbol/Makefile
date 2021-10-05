#
# Makefile --Build rules for "xtd", my extensions to the C library.
#
language = c
LIB_ROOT = ..
LOCAL.C_WARN_FLAGS = -Wno-switch-enum

C_SRC = enum.c symbol.c
H_SRC = symbol.h

include makeshift.mk library.mk

install: install-lib-include
uninstall: uninstall-lib-include
