
all: safe_string.o ctool.o 
	cc -o ctool safe_string.o ctool.o -lcurl -lssl -lcrypto

install: all
	/bin/mkdir -p /usr/local/bin
	/usr/bin/install -s ./ctool /usr/local/bin/

safe_string.o: safe_string.h safe_string.c
	cc -c safe_string.c

ctool.o: ctool.h ctool.c
	cc -c ctool.c

clean:
	rm -rf *.o
	rm -rf ctool
