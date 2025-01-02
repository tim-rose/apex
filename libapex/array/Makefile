#
# Makefile --Build rules for "apex/array", various slab/array ADTs.
#
language = c
LIB_ROOT = ..
subdir = apex

C_SRC = binsearch.c compare.c heap-sift.c heap.c lsearch.c pool.c \
    queue.c stack.c
H_SRC = array.h compare.h heap.h pool.h queue.h search.h stack.h

include makeshift.mk library.mk

install: install-lib-include
uninstall: uninstall-lib-include
