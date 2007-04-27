#!/bin/sh

cp=/bin/cp

flags='-Rp'

can-copy () {
    test -e $cp
}

version () {
    # Argh.  cp doesn't support --version!
    ls -l $cp
    echo
    echo "command = sudo $cp $flags src/* dst"
}

backup () {
    sudo $cp $flags $1/* $2
}
