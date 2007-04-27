#!/bin/sh

ditto=/usr/bin/ditto

flags='--rsrc'

can-copy () {
    test -e $ditto
}

version () {
    # Argh.  ditto doesn't support --version!
    ls -l $ditto
    echo
    echo "command = sudo $ditto $flags src dst"
}

backup () {
    sudo $ditto $flags $1 $2
}
