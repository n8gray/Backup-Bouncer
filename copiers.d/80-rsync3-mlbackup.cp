#!/bin/sh

rsync=/usr/local/maclemon/bin/rsync
# rsync=/usr/local/maclemon-beta/bin/rsync

# flags="-aH -E --rsync-path=$rsync"
# This should resemble what we use with mlbackup as closely as possible.
flags="-apogtxl --fileflags --protect-args --force-change --protect-decmpfs --crtimes --stats --hard-links -D --perms --executability --acls --xattrs --owner --group --times -v  --rsync-path=$rsync"

# NEW options we shoudl imply!
#   --devices               preserve device files (super-user only)
#	--specials              preserve special files
#		-D                          same as --devices --specials

# interesting:
#-O, --omit-dir-times        omit directories from --times
#    --super                 receiver attempts super-user activities
#    --fake-super            store/recover privileged attrs using xattrs




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
