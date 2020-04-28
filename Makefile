#
# Makefile --Build rules for "xtd", my collection of useful stuff.
#
PACKAGE	= libxtd
package-type = rpm deb

include devkit.mk version.mk package.mk

build@test:	build@libxtd
build@cmd:	build@libxtd
