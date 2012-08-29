all:
	cd util && make
clean:
	cd util && make clean
realclean:
	-umount /Volumes/Src
	-umount /Volumes/Dst
	-rm -f Src.sparseimage Dst.sparseimage
#
# Developer targets.  To use this:
# 1. Change ./VERSION to reflect the release version
# 2. Commit
# 3. make release-tag
# 4. make release
# 5. Change ./VERSION to reflect the post-release version
#
VERSION = $(shell cat ./VERSION)
NAME = backup-bouncer

release-tag:
	echo "Tagging release-$(VERSION)"
	TAGSDIR=`svn info | grep URL | awk '{print $$2}'`-tags \
	   && svn ls $$TAGSDIR \
	   | ( grep -q release-$(VERSION) \
	       || svn cp . $$TAGSDIR/release-$(VERSION) )
	
release:
	echo "Packaging version $(VERSION)"
	[ -d release ] || mkdir release
	TAGSDIR=`svn info | grep URL | awk '{print $$2}'`-tags \
	   && svn export $$TAGSDIR/release-$(VERSION) \
	                 release/$(NAME)-$(VERSION)
	cd release \
	   && tar cvzf $(NAME)-$(VERSION).tgz $(NAME)-$(VERSION)
	
upload:
	scp release/$(NAME)-$(VERSION).tgz \
	   n8gray.org:public_html/files/$(NAME)/

.PHONY: all clean release-tag release upload
