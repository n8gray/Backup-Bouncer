ctool_version=1.2.3

CFLAGS=-Wall

all: xattr-util hardlink-util
	(cd ctool-$(ctool_version); make)
	ln -fs ctool-$(ctool_version)/ctool .

clean:
	rm -f xattr-util hardlink-util
	(cd ctool-$(ctool_version); make clean)

.PHONY: all clean
