xar=/usr/local/dports/bin/xar

can-copy () {
    test -e $xar
}

version () {
    $xar --version
}

backup () {
    cd $1
    tmpfile=`mktemp -t backup-test-xar` || exit 1
    sudo $xar -c -f $tmpfile . \
        && cd $2 \
        && sudo $xar -x -f $tmpfile
    code=$?
    rm -f $tmpfile
    return $code
}
