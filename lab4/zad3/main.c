#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>


int main(int argc, char** argv) {
    if (argc < 2) {
        puts("main expects two argument");
        exit(0);
    }
    char * const mode = argv[1];
    char * const to_send = argv[2];

    pid_t pid;
    if ((pid = fork()) == 0) {
        char* const args[] = {mode, NULL};
        execvp("./catcher", args);
        exit(0);
    }

    char * catcher_pid = malloc(6);   // ex. 34567
    sprintf(catcher_pid, "%d", pid);

    if (fork() == 0) {
        char* const args[] = {catcher_pid, to_send, mode, NULL};
        execvp("./sender", args);
        exit(0);
    }

    wait(0);
    wait(0);
    free(catcher_pid);
}
