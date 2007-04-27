#!/bin/sh

pax=/bin/pax

flags='-p e'

can-copy () {
    test -e $pax
}

version () {
    ls -l $pax
    echo
    echo "command = cd src; sudo $pax -rw $flags . dst"
}

backup () {
    cd $1
    sudo $pax -rw $flags . $2
}
