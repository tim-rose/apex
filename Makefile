#
# Makefile --Build rules for "apex", my collection of useful stuff.
#
export VERSION ?= $(shell git describe --dirty 2>/dev/null)

PACKAGE	= libapex
package-type = rpm deb

include makeshift.mk package.mk

build@test:	build@libapex
build@demo:	build@libapex
