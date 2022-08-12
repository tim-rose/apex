#
# Makefile --Build rules for "xtd", my extensions to the C library.
#
language = c
LIB_ROOT = ..
subdir = xtd
LOCAL.C_WARN_FLAGS = -Wno-format-nonliteral -Wno-switch-enum

C_SRC = ini-parse.c ini-symbol.c log-parse.c nmea.c
H_SRC = ini.h log-parse.h nmea.h

include makeshift.mk library.mk
install: install-lib-include
uninstall: uninstall-lib-include
