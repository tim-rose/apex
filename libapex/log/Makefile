#
# Makefile --Build rules for "apex", my extensions to the C library.
#
language = c nroff markdown
LIB_ROOT = ..
subdir = apex
MAN3_SRC = log.3
C_SRC = config.c handler.c log-domain.c log.c message.c stderr.c \
    syslog.c
H_SRC = log-domain.h log.h

LOCAL.C_WARN_FLAGS = -Wno-format-security -Wno-format-nonliteral \
    -Wno-unused-but-set-variable -Wno-unreachable-code -Wno-type-limits \
    -Wno-strict-overflow

include makeshift.mk library.mk
install: install-lib-include install-lib-man
uninstall: uninstall-lib-include uninstall-lib-man
