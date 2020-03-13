#!/bin/sh
#
# mkdep --Find order dependencies between sub-directories of C code.
#
#
. midden
require log
#
#
# dir_list() --Output a list of directories, filtered from args.
#
# Remarks:
# This is a convenience function to filter out only the directories
# that are relevant.
#
dir_list()
{
    for d in "$@"; do
        if [ "$d" = "include" ]; then continue; fi
        if [ -d $d ]; then echo $d; fi
    done
}
#
#
# files_rgx() --Output a regex that will match a list of files.
#
# Parameters:
# dir --the directory containing the files
# glob --the file pattern defining "interesting" files
#
files_rgx() (
    local dir=$1 glob=$2; shift 2
    cd $dir >/dev/null
    echo $glob | sed -e 's/^/(/' -e 's/ /)|(/g' -e 's/[.]/[.]/g' -e 's/$/)/'
)
#
dirs=$(dir_list *)
for d in $dirs; do
    h_rgx=$(files_rgx "$d" '*.h')
    info 'dir: "%s", pattern: "%s"' "$d" "$h_rgx"
#
    for t in $dirs; do
        if [ "$t" = "$d" ]; then continue; fi
        if egrep "$h_rgx" $t/*.[ch] >/dev/null; then
            echo "build@$t: build@$d"
        fi
    done
done | sort
