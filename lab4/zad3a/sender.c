#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int received_from_catcher = 0;

void usr1handler(int signo, siginfo_t *info, void *context) {
    if (signo != SIGUSR1 && signo != SIGRTMIN+1){
        return;
    }
    received_from_catcher++;
}

void usr2handler(int signo, siginfo_t* info, void* ucontext) {
    if(signo != SIGUSR2 && signo != SIGRTMIN+2){
        return;
    }
    printf("sender received signo=%d: received from catcher: %d\n", signo, received_from_catcher);
    if (info->si_value.sival_int) {
        printf("(SIGQUEUE) sender: catcher received %d\n", info->si_value.sival_int);
    }
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        puts("sender expects 3 arguments");
        exit(0);
    }

    pid_t catcher_pid = atoi(argv[0]);
    int n = atoi(argv[1]);
    char* mode = argv[2];

    printf("MODE: %s\n", mode);

    struct sigaction actusr1;
    actusr1.sa_sigaction = usr1handler;
    actusr1.sa_flags = SA_SIGINFO;
    sigemptyset(&actusr1.sa_mask);
    if (strcmp(mode, "SIGRT") == 0){
        sigaction(SIGRTMIN+1, &actusr1, NULL);
    } else {
        sigaction(SIGUSR1, &actusr1, NULL);
    }

    struct sigaction actusr2;
    actusr2.sa_sigaction = usr2handler;
    actusr2.sa_flags = SA_SIGINFO;
    sigemptyset(&actusr2.sa_mask);
    if (strcmp(mode, "SIGRT") == 0){
        sigaction(SIGRTMIN+2, &actusr2, NULL);
    } else {
        sigaction(SIGUSR2, &actusr2, NULL);
    }


    if (strcmp(mode, "KILL") == 0) {
        for (int i = 0; i < n; ++i) {
            kill(catcher_pid, SIGUSR1);
        }
        kill(catcher_pid, SIGUSR2);
    }
    else if (strcmp(mode, "SIGQUEUE") == 0) {
        union sigval value;
        for (int i = 0; i < n; ++i) {
            value.sival_int = i;
            if (sigqueue(catcher_pid, SIGUSR1, value) != 0){
                puts("error sending sigueue");
            }
        }
        sigqueue(catcher_pid, SIGUSR2, value);
    }
    else if (strcmp(mode, "SIGRT") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGRTMIN+2);

        for (int i = 0; i < n; ++i) {
            kill(catcher_pid, SIGRTMIN+1);
        }
        kill(catcher_pid, SIGRTMIN+2);

        // wait for SIGRTMIN+2
        sigsuspend(&mask);

        printf("sender: sent %d \n", n);
        printf("sender: Should receive %d\n", received_from_catcher);
        exit(0);
    }
    else {
        puts("unknown command");
        exit(0);
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    // wait for SIGUSR2
    printf("sender: sent %d \n", n);
    sigsuspend(&mask);
    printf("sender: Should receive %d\n", received_from_catcher);
}

