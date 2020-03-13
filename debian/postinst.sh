#!/bin/sh -e
#
# POSTINST --Debian "post-install" script for the adminkit.
#
#
# Remarks:
# This just forces the wakeup program to be setuid root.
#
# See Also:
# prerm.sh, Makefile
#
script=$(basename $0)
package=adminkit

#
# main...
#
case $1 in
    configure)
	chown root:root /usr/bin/wakeup
	chmod u+s /usr/bin/wakeup
	;;
    abort-upgrade|abort-deconfigure|abort-remove)	:;;
    *) echo "$script: Unrecognised command \"$1 \"" >&2; exit 2;;
esac

exit 0
