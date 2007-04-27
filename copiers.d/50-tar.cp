#!/bin/sh

tar=/usr/bin/tar

can-copy () {
    test -e $tar
}

version () {
    $tar --version
    echo
    echo "command = sudo $tar -cf - -C src . | sudo $tar -x --preserve -f - -C dst"
}

backup () {
    sudo $tar -cf - -C $1 . | sudo $tar -x --preserve -f - -C $2
}
