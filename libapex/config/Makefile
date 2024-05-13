#
# Makefile --Build rules for "apex/config", my extensions to the C library.
#
language = c
LIB_ROOT = ..
subdir = apex
C_SRC = config.c convert.c getopts.c log-getopt.c opt-parse.c \
    option.c
H_SRC = config.h convert.h getopts.h option.h

include makeshift.mk library.mk

install: install-lib-include
uninstall: uninstall-lib-include
