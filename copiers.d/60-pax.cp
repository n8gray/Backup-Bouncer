pax=/bin/pax

flags='-p e'

can-copy () {
    test -e $pax
}

version () {
    ls -l $pax
}

backup () {
    cd $1
    sudo $pax -rw $flags . $2
}
