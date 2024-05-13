#
# Makefile --Build rules for "apex", my extensions to the C library.
#
language = c
LIB_ROOT = ..
subdir = apex

C_SRC = hash.c key-elf.c key-jenkins.c key-pjw.c keyn-elf.c \
    keyn-jenkins.c keyn-pjw.c
H_SRC = hash.h

include makeshift.mk library.mk

install: install-lib-include
uninstall: uninstall-lib-include
