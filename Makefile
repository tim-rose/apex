#
# Makefile --Build rules for "xtd", my collection of useful stuff.
#
PACKAGE	= libxtd
package-type = rpm deb
language = markdown
MD_SRC = README.md
MK_SRC = _VERSION.mk

include devkit.mk version.mk package.mk

#$(PVRA).rpm: $(PACKAGE).spec $(PACKAGE)-files.txt
#	/usr/bin/rpmbuild --define "_topdir $$PWD/rpm" -bb --clean $(PACKAGE).spec
#	mv  $(ARCH)/$(PVRA).rpm .
#	test -f $(ARCH)/$(PACKAGE)-debuginfo-$(VRA).rpm && \
#	    mv $(ARCH)/$(PACKAGE)-debuginfo-$(VRA).rpm .; true
#	rmdir $(ARCH)
#
#RM_PREFIX       = -e 's|^.data||'
#QUOTE_LINE      = -e 's/.*/"&"/'
#DOC_ADJUST      = -e '/man[0-9]/s/^/%doc /'
##CONF_ADJUST    = -e '/.conf"/s/^/%config(noreplace) /'
#$(PACKAGE)-files.txt:   $(C_SRC) $(TEST_SRC) $(Y_SRC) $(L_SRC) $(H_SRC)
#$(PACKAGE)-files.txt:   $(PL_SRC) $(PM_SRC) $(T_SRC) $(SH_SRC)
#$(PACKAGE)-files.txt:   .data
#	find .data -not -type d | \
#            sed $(RM_PREFIX) $(QUOTE_LINE) $(DOC_ADJUST) $(CONF_ADJUST) >$@
#
build@test:	build@libxtd
build@cmd:	build@libxtd
