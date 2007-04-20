rsync=/usr/bin/rsync

flags='-aH -E'

version () {
    $rsync --version
}

backup () {
    $rsync $flags $1/ $2
}
