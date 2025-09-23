#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: sleep TICKS\n");
        exit(1);
    }

    int ticks = atoi(argv[1]);
    if (ticks <= 0)
        exit(0);

    if (pause(ticks) < 0) {
        printf("sleep: pause failed\n");
        exit(1);
    }

    exit(0);
}


