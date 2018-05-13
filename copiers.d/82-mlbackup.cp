#!/bin/sh

mlbackup=/usr/local/bin/mlbackup

# flags="-aH -E --rsync-path=$rsync"
# This should resemble what we use with mlbackup as closely as possible.


# Should exit with code 0 if the necessary programs exist, 1 otherwise
can-copy () {
    test -e $mlbackup
}

# Should generate some text on stdout identifying which version of the
# copier is being used, and how it's called.  This is optional.
version () {
    $mlbackup
    echo
    echo "command = sudo $mlbackup $config"
}

# Should perform a copy from $1 to $2.  Both will be directories.  Neither
# will end with '/'.  So you'll get something like:
#   backup /Volumes/Src /Volumes/Dst/99-foo
backup () {
    config=$(mktemp -t backupbouncer-82-mlbackup-.XXXXXXXXXX)

    echo "# mlbackup backup bouncer configuration file
# -----------------------------------------------------------------------------
MLdebug=1
MLbeVerbose="-vvv"
MLuseGlobalExclusions=0
# -----------------------------------------------------------------------------

# What is the name of this backup set?
MLbackupName="82-mlbackup"

# NOTE on Naming Backups:
# Spaces are allowed in Backup Set Names. (tested and verified)
# If you save this script with UTF-8 Encoding, it should
# be capable of handling japanese characters (tested and verified)
# This script is not yet capable of handling Umlauts correctly!
# If you know how to make this work, please contact me!
# -----------------------------------------------------------------------------
# How many Backup Sets to keep? (Integer > 0)
MLbackupCount=1

# -----------------------------------------------------------------------------
# Configuring the SOURCE that will be backed up:

# What file or directory to Backup. (No trailing Slash for directories)
# Use quotes if spaces are in your path.
MLsourcePath="/Volumes/src"

# -----------------------------------------------------------------------------
# Configuring the Destination we will save the Backup to.

# Where shall the Backups be stored (local path on the destination machine)
MLdestPath="/Volumes/Dst/82-mlbackup"

# -----------------------------------------------------------------------------

# eMail Address to send the report to
MLadminEmail=""

# MLcustomRsyncOptions
#EOF
" > $config
    sudo $mlbackup $config
    returnvalue=$?
    # mlbackuptarget=$(ls -1 /Volumes/Dst/82-mlbackup/82-mlbackup-2011*)

    mv /Volumes/Dst/82-mlbackup/82-mlbackup-*/src/* /Volumes/Dst/82-mlbackup/

    rm "$config"
    return $returnvalue 
}
