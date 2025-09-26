#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: sixfive <file>\n");
        exit(1);
    }

    int fd = open(argv[1],0);
    if (fd < 0) {
        printf("Could not open file %s\n", argv[1]);
        exit(1);
    }

    char seps[] = " -\r\t\n./,";
    char token[256];
    int len = 0;
    int alldigits = 1;
    char ch;

    while (read(fd, &ch, 1) == 1) {
        if (strchr(seps, ch)) {
            if (len > 0) {
                token[len] = '\0';
                if (alldigits) {
                    int num = atoi(token);  // xv6 has its own atoi
                    if (num % 5 == 0 || num % 6 == 0) {
                        printf("%d\n", num);
                    }
                }
                len = 0;
                alldigits = 1;
            }
        } else {
            token[len++] = ch;
            if (!(ch >= '0' && ch <= '9')) {
                alldigits = 0;
            }
        }
    }

    if (len > 0) {
        token[len] = '\0';
        if (alldigits) {
            int num = atoi(token);
            if (num % 5 == 0 || num % 6 == 0) {
                printf("%d\n", num);
            }
        }
    }

    close(fd);
    exit(0);
}

