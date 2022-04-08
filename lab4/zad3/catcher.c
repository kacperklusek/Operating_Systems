#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int received = 0;
pid_t sender_pid;


void usr1handler(pid_t signo) {
    if (signo != SIGUSR1 && signo != SIGRTMIN+1){
        return;
    }
    received++;
}

void usr2handler(pid_t signo, siginfo_t *info, void* context) {
    if(signo != SIGUSR2 && signo != SIGRTMIN+2){
        return;
    }
    puts("catcher: received SIGUSR2");
    sender_pid = info->si_pid;
}

int main(int argc, char* argv[]) {
    char * const mode = argv[0];

    struct sigaction actusr1;
    actusr1.sa_handler = usr1handler;
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

    sigset_t mask;
    sigemptyset(&mask);
    if (strcmp(mode, "SIGRT") == 0) {
        sigaddset(&mask, SIGRTMIN+2);
    } else {
        sigaddset(&mask, SIGUSR2);
    }
    // wait for SIGUSR2 or SIGRTMIN+2
    sigsuspend(&mask);

    if (strcmp(mode, "KILL") == 0) {
        for (int i = 0; i < received; ++i) {
            kill(sender_pid, SIGUSR1);
        }
        kill(sender_pid, SIGUSR2);   
    }
    else if (strcmp(mode, "SIGQUEUE") == 0) {
        union sigval value;
        for (int i = 0; i < received; ++i) {
            value.sival_int = i;
            sigqueue(sender_pid, SIGUSR1, value);
        }
        value.sival_int = received;
        sigqueue(sender_pid, SIGUSR2, value);
    }
    else if (strcmp(mode, "SIGRT") == 0) {
        for (int i = 0; i < received; ++i) {
            kill(sender_pid, SIGRTMIN+1);
        }
        kill(sender_pid, SIGRTMIN+2);
    }

    printf("catcher: received  %d\n", received);

}

