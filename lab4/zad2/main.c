#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

void handler(int pid) {
    printf("SIGCHLD!\n");
}

void test_nocldstop() {
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = SA_NOCLDSTOP;

    sigaction(SIGCHLD, &act, NULL);

    int pid;
    if ((pid = fork()) == 0) {
        raise(SIGSTOP);
        exit(0);
    }

    sleep(2);
    kill(pid, SIGCONT);

    wait(0);
}

void info_handler(int sig, siginfo_t *info, void *ucontext) {
    printf("sig: %d \t si_pid: %d \t architecture: %ud", sig, info->si_pid, info->si_arch);
}

void test_siginfo() {
    struct sigaction act;
    act.sa_sigaction = info_handler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGCHLD, &act, NULL);

    if (fork() == 0) {
        exit(0);
    }

    wait(0);
}

void nodefer_handler(int signum){
    raise(SIGUSR1);
    printf("succesfully blocked SIGUSR1\n");
}

void test_nodefer() {
    struct sigaction act;
    act.sa_handler = nodefer_handler;
    act.sa_flags = SA_NODEFER;

    sigaction(SIGUSR1, &act, NULL);

    if (fork() == 0) {
        raise(SIGUSR1);
        exit(0);
    }

    wait(0);
}

int main(int argc, char** argv) {
    test_nocldstop();
    test_siginfo();
    test_nodefer();
}
