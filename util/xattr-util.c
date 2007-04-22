#include <sys/xattr.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        fprintf(stderr,"Usage: %s [l | r attr | w attr val] file\n", argv[0]);
        return 1;
    }
    
    int options = XATTR_NOFOLLOW;
    if (argv[1][0] == 'r') {
        /* Read mode */
        if (argc != 4) {
            fprintf(stderr, "Wrong number of arguments\n");
            return 1;
        }
        char buf[160];
        int size=getxattr(argv[3], argv[2], &buf[0], 159, 0, options);
        if (size < 0) {
            perror(argv[0]);
            return 1;
        }
        buf[159] = '\0';
        printf(buf);
        return 0;
    } 
    else if (argv[1][0] == 'w') {
        /* Write mode */
        if (argc != 5) {
            fprintf(stderr, "Wrong number of arguments\n");
            return 1;
        }
        int code = setxattr(argv[4], argv[2], argv[3], strlen(argv[3])+1, 0,
                            options);
        if (code < 0) {
            perror(argv[0]);
            return 1;
        }
        return 0;
    }
    else if (argv[1][0] == 'l') {
        if (argc != 3) {
            fprintf(stderr, "Wrong number of arguments\n");
            return 1;
        }
        size_t size = listxattr(argv[2], NULL, 0, options);
        if (size == 0) {
            return 0;
        }
        if (size < 0) {
            perror(argv[0]);
            return 1;
        }
        char *buf = (char *)malloc(size);
        size = listxattr(argv[2], buf, size, options);
        if (size < 0) {
            perror(argv[0]);
            return 1;
        }
        char *end = buf + size;
        while(buf < end) {
            printf("%s\n", buf);
            buf += strlen(buf) + 1;
        }
        return 0;
    }
    else {
        fprintf(stderr, "Bad arguments\n");
        return 1;
    }
}
