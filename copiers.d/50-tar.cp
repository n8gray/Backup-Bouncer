tar=/usr/bin/tar

can-copy () {
    test -e $tar
}

version () {
    $tar --version
}

backup () {
    sudo $tar -cf - -C $1 . | sudo $tar -x --preserve -f - -C $2
}
