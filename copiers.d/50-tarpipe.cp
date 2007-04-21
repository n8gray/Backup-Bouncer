tar=/usr/bin/tar

flags='-Rp'

version () {
    $tar --version
}

backup () {
    sudo $tar -cf - -C $1 . | $tar -x --preserve -f - -C $2
}
