cp=/bin/cp

flags='-Rp'

version () {
    # Argh.  cp doesn't support --version!
    ls -l $cp
}

backup () {
    sudo $cp $flags $1/* $2
}
