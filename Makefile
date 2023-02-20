#
# Makefile --Build rules for "xtd", my collection of useful stuff.
#
export VERSION ?= $(shell git describe --dirty 2>/dev/null)

PACKAGE	= libxtd
package-type = rpm deb

include makeshift.mk package.mk

build@test:	build@libxtd
build@demo:	build@libxtd
