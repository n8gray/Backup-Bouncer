rsync=/usr/bin/rsync

flags='-aH -E'

can-copy () {
    test -e $rsync
}

version () {
    $rsync --version
}

backup () {
    sudo $rsync $flags $1/ $2
}
