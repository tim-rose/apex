#
# Makefile --Build rules for the string components of apex.
#
language = c
LIB_ROOT = ..
subdir = apex

C_SRC = estring.c memswap.c stredit.c strlist.c strparse.c
H_SRC = estring.h strparse.h

include makeshift.mk library.mk

# override warnings for winnt inet_pton... 
ifeq ($(OS),mingw64_nt)
$(archdir)/strparse.o: TARGET.CFLAGS = -Wno-discarded-qualifiers 
endif

install: install-lib-include
uninstall: uninstall-lib-include
