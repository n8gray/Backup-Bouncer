rsync=/usr/bin/rsync

flags='-aH -E'

version () {
    $rsync --version
}

backup () {
    sudo $rsync $flags $1/ $2
}
