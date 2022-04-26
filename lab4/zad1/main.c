#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

void handler(int signum) {
    printf("handling signal no. %d \n", signum);
}

int main(int argc, char** argv) {
    if (argc != 2){
        puts("main expects exactly 2 arguments");
        exit(0);
    }

    char* command = argv[1];


    if (strcmp(command, "ignore") == 0) {
        // ignore SIGUSR1
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
        fflush(stdout);

        if (fork() == 0) {
            raise(SIGUSR1);
            puts("SIGUSR1 is being ignored in child process");
            exit(0);
        }
        wait(0);

        char* const args[2] = {"main", "exec-ignore"};
        // https://i.stack.imgur.com/RmfZk.jpg
        fflush(stdout);
        if (fork() == 0) {
            execvp("./main", args);
            exit(0);
        }
        wait(0);
    }
    else if (strcmp(command, "handler") == 0) {
        // handle SIGUSR1
        signal(SIGUSR1, handler);
        raise(SIGUSR1);
        fflush(stdout);

        if (fork() == 0) {
            puts("child process:");
            raise(SIGUSR1);
            exit(0);
        }
        wait(0);
    }
    else if (strcmp(command, "mask") == 0) {
        // mask SIGUSR1
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        raise(SIGUSR1);

        sigset_t set;
        sigpending(&set);
        if (sigismember(&set, SIGUSR1) ) {
            printf("SIGUSR1 waiting after being masked\n");
        }

        fflush(stdout);
        if (fork() == 0) {
            raise(SIGUSR1);
            sigset_t child_set;
            sigpending(&child_set);
            if (sigismember(&child_set, SIGUSR1)){
                puts("SIGUSR1 is masked in child process");
            }
            exit(0);
        }
        wait(0);

        char* const args[2] = {"main", "exec-mask"};
        // https://i.stack.imgur.com/RmfZk.jpg
        fflush(stdout);
        if (fork() == 0) {
            execvp("./main", args);
            exit(0);
        }
        wait(0);
    }
    else if (strcmp(command, "pending") == 0) {
        // mask SIGUSR1
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        raise(SIGUSR1);

        sigset_t set;
        sigpending(&set);
        if (sigismember(&set, SIGUSR1) ) {
            puts("SIGUSR1 waiting after being masked");
        }

        fflush(stdout);
        if (fork() == 0) {
            sigset_t child_set;
            sigpending(&child_set);
            if (! sigismember(&child_set, SIGUSR1)){
                puts("parent's SIGUSR1 is not pending in child process");
            }
            exit(0);
        }
        wait(0);

        char* args[2] = {"main", "exec-pending"};
        // https://i.stack.imgur.com/RmfZk.jpg
        fflush(stdout);
//        if (fork() == 0) {
            execvp("./main", args);
//            exit(0);
//        }
//        wait(0);
    }
    else if(strcmp(command, "exec-ignore") == 0) {
        raise(SIGUSR1);
        puts("SIGUSR1 successfully ignored in exec process");
    }
    else if(strcmp(command, "exec-mask") == 0) {
        raise(SIGUSR1);
        sigset_t set;
        sigpending(&set);
        if (sigismember(&set, SIGUSR1) ) {
            puts("SIGUSR1 is being masked in exec child process");
        }
    }
    else if(strcmp(command, "exec-pending") == 0) {
        sigset_t set;
        sigpending(&set);
        if (! sigismember(&set, SIGUSR1) ) {
            puts("parent's SIGUSR1 is not pending in exec child process");
        } else {
            puts("parent's SIGUSR1 IS pending in exec child process");
        }
    }
}
