#include <sys/xattr.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int show_xattr(char *file, char *name)
{
    int options = XATTR_NOFOLLOW;
    int size = getxattr(file, name, NULL, 0, 0, options);
    if (size < 0) {
        return 0;
    }
    void *result = malloc(size);
    if (NULL != result) {
        size_t read_size = getxattr(file, name, result, size, 0, options);
        if (read_size >= 0)  {
            write(1, result, read_size);
            free(result);
            return 1;
        }
        free(result);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        fprintf(stderr,"Usage: %s [l | a | r attr | w attr val] file\n", 
                argv[0]);
        return 1;
    }
    
    int options = XATTR_NOFOLLOW;
    if (argv[1][0] == 'r') {
        /* Read mode */
        if (argc != 4) {
            fprintf(stderr, "Wrong number of arguments\n");
            return 1;
        }
        if (show_xattr(argv[3], argv[2]))
            return 0;
        perror(argv[0]);
        return 1;
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
    else if (argv[1][0] == 'a') {
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
        void *value;
        while(buf < end) {
            printf("%s: \"", buf);
            fflush(stdout);
            if (!show_xattr(argv[2], buf)) {
                perror(argv[0]);
                return 1;
            }
            printf("\"\n");
            buf += strlen(buf) + 1;
        }
        return 0;
    }
    else {
        fprintf(stderr, "Bad arguments\n");
        return 1;
    }
}
