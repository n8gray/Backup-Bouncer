rsync=/usr/local/bin/rsync+xattr

flags='-aH -E'

can-copy () {
    test -e $rsync
}

version () {
    echo 'Rsync "fixed for Tiger" from http://www.onthenet.com.au/~q/rsync/'
    $rsync --version
}

backup () {
    sudo $rsync $flags $1/ $2
}
