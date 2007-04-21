pax=/bin/pax

flags='-p e'

version () {
    ls -l $pax
}

backup () {
    cd $1
    sudo $pax -rw $flags . $2
}
