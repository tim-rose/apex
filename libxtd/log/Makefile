#
# Makefile --Build rules for "xtd", my extensions to the C library.
#
language = c nroff markdown
LIB_ROOT = ..
subdir = xtd

MMD_SRC = liblog.mmd
MAN3_SRC = log.3
C_SRC = config.c handler.c log-domain.c log.c message.c stderr.c \
    syslog.c
H_SRC = log-domain.h log.h

LOCAL.C_WARN_FLAGS = -Wno-format-security -Wno-format-nonliteral

include makeshift.mk library.mk
install: install-lib-include install-lib-man
uninstall: uninstall-lib-include uninstall-lib-man
