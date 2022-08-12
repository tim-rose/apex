#
# Makefile --Build rules for "tap", my extensions to the C library.
#
# Remarks:
# Take a look at:
# git@github.com:zorgnax/libtap.git
# mock frameworks:
# https://github.com/ThrowTheSwitch/CMock/blob/master/docs/CMock_Summary.md
# http://cmockery.googlecode.com/svn/trunk/doc/index.html
# https://cmocka.org
#
language = c
LIB_ROOT = ..
subdir = xtd
LOCAL.C_WARN_FLAGS = -Wno-format-nonliteral

C_SRC = tap.c
H_SRC = tap.h test.h

include makeshift.mk library.mk
install: install-lib-include
uninstall: uninstall-lib-include
