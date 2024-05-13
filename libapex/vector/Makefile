#
# Makefile --Build rules for "apex", my extensions to the C library.
#
language = c
LIB_ROOT = ..
subdir = apex
LOCAL.C_WARN_FLAGS	= -Wno-cast-align

C_SRC = vector.c
H_SRC = vector.h

include makeshift.mk library.mk

install: install-lib-include
uninstall: uninstall-lib-include
