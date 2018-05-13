#!/bin/sh

restic=$HOME/bin/restic

# flags="-aH -E --rsync-path=$rsync"
# This should resemble what we use with mlbackup as closely as possible.


# Should exit with code 0 if the necessary programs exist, 1 otherwise
can-copy () {
    test -e $restic
}

# Should generate some text on stdout identifying which version of the
# copier is being used, and how it's called.  This is optional.
version () {
    $restic version
}

# Should perform a copy from $1 to $2.  Both will be directories.  Neither
# will end with '/'.  So you'll get something like:
#   backup /Volumes/Src /Volumes/Dst/99-foo
backup () {
    restictempdir=$(mktemp -d -t backupbouncer-85-restic-.XXXXXXXXXX)

    echo init restic repo
    RESTIC_PASSWORD="test" "$restic" init --repo "$restictempdir"
    echo make restic backup of Src
    RESTIC_PASSWORD="test" "$restic" --repo "$restictempdir" backup /Volumes/Src
    echo restore data from restic backup to Dst
    RESTIC_PASSWORD="test" "$restic" --repo "$restictempdir" restore latest --target /Volumes/Dst/85-restic
    returnvalue=$?
    echo Move files where BackupBouncer expects them for checks
    mv /Volumes/Dst/85-restic/Src/* /Volumes/Dst/85-restic/
    echo clean up temp directory
    unset RESTIC_PASSWORD
    return $returnvalue
}
