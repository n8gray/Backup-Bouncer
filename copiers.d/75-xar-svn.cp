#!/bin/sh

# This is where I've installed the SVN head version of xar
# updated on 12/13/07
xar=/usr/local/bin/xar

can-copy () {
    test -e $xar
}

version () {
    $xar --version
    echo
    echo "command = 
    cd src
    sudo $xar -c -f tmpfile . \
        && cd dst \
        && sudo $xar -x --keep-setuid -P -f $tmpfile"
}

backup () {
    cd $1
    # xar doesn't work with pipes yet, so we use a tmpfile
    tmpfile=`mktemp` || exit 1    # Need to use mktemp correctly!
    sudo $xar -c -f $tmpfile . \
        && cd $2 \
        && sudo $xar -x --keep-setuid -P -f $tmpfile
    code=$?
    rm -f $tmpfile
    return $code
}
