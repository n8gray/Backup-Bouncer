Backup Bouncer
==============

A unit-testing framework for OS X backup tools.
Written by Nathaniel Gray.
n8gray /at/ n8gray \dot\ org

About
=====

This is a framework for systematically testing backup tools in OS X, with a
focus on ensuring proper preservation of file-related metadata.  It contains a
suite of tests to verify that metadata has survived a copy operation, along with
a suite of copiers.  It will even automatically prepare disk images as
source/destination targets for you.

Prerequisites
=============

In order to examine/set Finder metadata attributes, the following utilities are
required:

* SetFile
* GetFileInfo

These are included with the XCode developer tools, and bbouncer expects them
to be in /Developer/Tools.  If they're not there for some reason, add their
location to your PATH and all will be well.

Installation & Usage
====================

"Installation" is simple
1. run "make"
2. there's no step 2.

To be clear, you do NOT install backup bouncer, you simply run it from the place
you unpacked it.

You can now run "bbouncer" with no arguments for usage info.  Or, you can run
"./autopilot" to automatically generate some disk images, run all tests, and see
the results.  Also take a look in the scripts directory for scripts you can use
to test tools that can't be run in the normal test suite for whatever reason.

Note that many of the tests and all of the copiers require root priveleges.  
They run using sudo, so you'll be prompted for your password.  This also means
you need to run the suite as a member of the admin group.

One more thing.  As with most Bash scripts, this isn't guaranteed to handle
pathnames with spaces well.  So please, don't put spaces in your test volume
names!

Limitations
===========

For the moment, Backup Bouncer is focused on validating archival tools used in
whole-subtree copying mode.  This means that our automated tests don't test how
well metadata is preserved for single-file copies (in tools that support them). 
This is an issue, for example, for metadata that are stored in .DS_Store files. 
A whole-subtree copy will copy the .DS_Store file along with the directory
contents, but a single-file copy of a file may not preserve metadata from the
.DS_Store file.  This is an important problem for incremental backup tools, but
not one addressed by the automatic tests in Backup Bouncer at this time.

Having said that, BB can test *any* backup of a BB volume against its source, so you can test single-file copies by:
    1. Copying with a known-good tool (like SuperDuper)
    2. Making some changes to a file on the source volume
    3. Copying the file
    4. Running "bbouncer verify" on the source/destination volumes

Be aware that this program is alpha-quality software.  You're encouraged to use
the autopilot script to make sure you don't stray too far from what the program
expects.

Adding Tests
============

Adding tests is easy!  Just copy an existing test and fill in the functions.
Take a look at tests.d/00-basic-permissions.test for comments on the arguments
and expected behavior of each function.

After you add a test you need to re-run 'bbouncer create' on your source
volume.

Adding Copiers
==============

Adding copiers is easy too!  Again, just copy an existing .cp file and fill
in the functions.  See copiers.d/10-rsync-apple.cp for comments on the arguments
and expected behavior of each function.

You should understand that you don't have to add a copier to test your favorite
copy mechanism.  You can do the copy by hand and then use 'bbouncer verify'
to do the verification.  Copiers are just there to ease automated testing.

BUGS
====

We need a test for Finder aliases.

This should be written in OMake (omake.metaprl.org) rather than Bash.  What was
I thinking???

There should be an option for copy to automatically re-populate a src volume if
it's stale.

History
=======

This work was inspired by the plasticsfuture blog post[1] on the (rather sad)
state of OS X backup tools.  The post was an extremely valuable contribution in
many ways, but it was particularly helpful for simply enumerating all (?) of the
many and varied metadata attributes of a modern Mac HFS+ volume.  However,
despite requests from myself and others, maurits neglected to release his test
suite to the public.

The topic of backup fidelity became important to me personally as the hard drive
in my powerbook started slowly dying the death of a thousand bad blocks.  So I
set out to reproduce maurits' results and run some extra tests of my own.

References
==========
[1] The post that started it all: 
    <http://blog.plasticsfuture.org/2006/03/05/the-state-of-backup-and-cloning-tools-under-mac-os-x/>

[2] A nice, though not 100% accurate site on OS X filesystem metadata.  Includes references!
    <http://www.macos-x-server.com/wiki/index.php?title=Filesystem_metadata_support>

