#!/bin/sh

rsync=/usr/local/bin/rsync+xattr

flags="-aH -E --rsync-path=$rsync"

can-copy () {
    test -e $rsync
}

version () {
    echo 'Rsync "fixed for Tiger" from http://www.onthenet.com.au/~q/rsync/'
    $rsync --version
    echo
    echo "command = sudo $rsync $flags src/ dst"
}

backup () {
    sudo $rsync $flags $1/ $2
}
