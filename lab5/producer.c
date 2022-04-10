#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "errno.h"

#define WAIT_TIME 1

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("producer needs 4 arguments, %d given\n", argc);
        return 0;
    }

    char* pipe_path = argv[1];
    int row_number = atoi(argv[2]);
    char* file_path = argv[3];
    int buff_size = atoi(argv[4]);

    FILE *prod_pipe;
    if ((prod_pipe = fopen(pipe_path, "w")) == NULL) {
        printf("ERROR opening pipe: %d", errno);
        return 1;
    };
    FILE *in_file = fopen(file_path, "r");
    char buff[buff_size];
    char write_buff[buff_size];

    while (fgets(buff, buff_size, in_file) != NULL) {
        sleep(WAIT_TIME);
        sprintf(write_buff, "%d %s\n", row_number, buff);
        fputs(write_buff, prod_pipe);
        puts(write_buff);
    }

    fclose(prod_pipe);
    fclose(in_file);

    return 0;
}