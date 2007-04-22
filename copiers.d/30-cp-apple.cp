cp=/bin/cp

flags='-Rp'

can-copy () {
    test -e $cp
}

version () {
    # Argh.  cp doesn't support --version!
    ls -l $cp
}

backup () {
    sudo $cp $flags $1/* $2
}
