#!/bin/sh

rsync=/sw/bin/rsync

flags="-aNAHXx --fileflags --force-change"

# Should exit with code 0 if the necessary programs exist, 1 otherwise
can-copy () {
    test -e $rsync
}

# Should generate some text on stdout identifying which version of the
# copier is being used, and how it's called.  This is optional.
version () {
    $rsync --version
    echo
    echo "command = sudo $rsync $flags src/ dst"
}

# Should perform a copy from $1 to $2.  Both will be directories.  Neither
# will end with '/'.  So you'll get something like:
#   backup /Volumes/Src /Volumes/Dst/99-foo
backup () {
    sudo $rsync $flags $1/ $2
}
