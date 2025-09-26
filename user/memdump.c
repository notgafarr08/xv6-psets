#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
memdump(char *fmt, char *data)
{
    for (; *fmt; fmt++) {
        char f = *fmt;

        switch (f) {
        case 'i': {
            int val = *(int*)data;
            printf("%d\n", val);
            data += 4;
            break;
        }
        case 'h': {
            short val = *(short*)data;
            printf("%d\n", val);
            data += 2;
            break;
        }
        case 'c': {
            char val = *data;
            printf("%c\n", val);
            data += 1;
            break;
        }
        case 'p': {
            long long val = *(long long*)data;
            printf("%llx\n", val);
            data += 8;
            break;
        }
        case 's': {
            char *str = *(char **)data;
            printf("%s\n", str);
            data += 8;
            break;
        }
        case 'S': {
            char *str = (char *)data;
            printf("%s\n", str);
            return;
        }
        default:
            printf("Unknown format char: %c\n", f);
            break;
        }
    }
}

int
main(int argc, char *argv[])
{
    if(argc == 1){
        // Built-in examples from assignment
        printf("Example 1:\n");
        int a = 61810;
        int b = 2025;
        memdump("i", (char*)&a);
        memdump("i", (char*)&b);

        printf("Example 2:\n");
        memdump("S", "a string");

        printf("Example 3:\n");
        memdump("S", "another");

        printf("Example 4:\n");
        struct {
            char c1;
            char c2;
            short h;
            int i;
            char *s;
        } example4 = { 'B', 'D', 0, 1819438967, "xyzzy" };
        memdump("cc hi s", (char*)&example4);

        printf("Example 5:\n");
        memdump("S", "hello");
        memdump("c", "w");
        memdump("c", "o");
        memdump("c", "r");
        memdump("c", "l");
        memdump("c", "d");

    } else {
        // Command-line mode: read stdin as raw data
        char buf[512];
        int n = read(0, buf, sizeof(buf));
        if (n < 0) {
            printf("memdump: read error\n");
            exit(1);
        }
        memdump(argv[1], buf);
    }

    exit(0);
}

