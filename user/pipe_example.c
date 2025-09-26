#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int p[2];
    char *argv[2];

    argv[0] = "wc";
    argv[1] = 0;

    pipe(p);
    if (fork() == 0) {
        // Child: make pipe read end become stdin (fd 0)
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        exec("wc", argv);
        exit(1); // exec only returns if there is an error
    } else {
        // Parent: write into pipe
        close(p[0]);
        write(p[1], "hello world\n", 12);
        close(p[1]);
    }
    exit(0);
}

