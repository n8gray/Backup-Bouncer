ctool_version=1.2.3

all: xattr-util
	(cd ctool-$(ctool_version); make)
	ln -fs ctool-$(ctool_version)/ctool .

clean:
	rm -f xattr-util
	(cd ctool-$(ctool_version); make clean)

.PHONY: all clean
