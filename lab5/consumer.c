#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        puts("consumer needs 3 arguments");
        return 0;
    }

    char* pipe_path = argv[1];
    char* file_path = argv[2];
    int buff_size = atoi(argv[3]);

    FILE * prod_pipe;
    FILE * out_file;
    if ((prod_pipe = fopen(pipe_path, "r")) == NULL) {
        puts("ERROR opening pipe");
        return 1;
    };
    if ((out_file = fopen(file_path, "w")) == NULL) {
        puts("ERROR opening file");
        return 1;
    };
    char buff[buff_size];
    int row_number;

    while (fgets(buff, buff_size, prod_pipe) != NULL) {
        sscanf(buff, "%d", &row_number);
        fputs(buff, out_file);
    }

    fclose(prod_pipe);
    fclose(out_file);

    return 0;
}

