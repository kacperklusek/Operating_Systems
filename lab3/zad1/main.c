#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int n = atoi(argv[1]);

    for (int i = 0; i < n; ++i) {
        if (fork() == 0) {
            printf("napis pochodzi z procesu o PID=%d\n", getpid());
            exit(0);
        }
    }

    return 0;
}

