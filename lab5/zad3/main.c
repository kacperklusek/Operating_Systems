#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFF_SIZE 512
#define MAIL_CMD_SIZE 128
#define WRITE_PIPE 1
#define READ_PIPE 0
#define EMAIL_COLUMN 2
#define DATE_COLUMN 3


int main(int argc, char* argv[]) {
    const char* fifoname = "./fifo_pipe";
    const int PROD_NUM = 2;

    if (mkfifo(fifoname, 0666) == -1) {
        printf("mkfifo error %d\n", errno);
        return 1;
    }

    if (fork() == 0) {
        if (execlp("./consumer", "./consumer", fifoname, "out_file.txt", "30", (char *) NULL) == -1) {
            printf("ERROR consumer [%d]\n", errno);
        }
        exit(0);
    }

    char buf[16];
    char num[3];
    for(int i = 0; i < PROD_NUM; ++i){
        sprintf(buf, "prod%d.txt", i+1);
        sprintf(num, "%d", i+1);
        if(fork() == 0){
            if (execlp("./producer", "./producer", fifoname, num, buf, "30", (char *) 0) == -1) {
                printf("ERROR producer [%d]\n", errno);
            }
            exit(0);
        }
    }

    for (int i = 0; i < PROD_NUM+1; ++i) {
        wait(0);
    }

    puts("end");

    return 0;
}
