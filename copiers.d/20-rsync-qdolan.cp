rsync=/usr/local/bin/rsync+xattr

flags='-aH -E'

version () {
    echo 'Rsync "fixed for Tiger" from http://www.onthenet.com.au/~q/rsync/'
    $rsync --version
}

backup () {
    $rsync $flags $1/ $2
}
