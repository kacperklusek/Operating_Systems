#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "stdlib.h"



int* sys_count(char target, char*path) {
    int file = open(path, O_RDONLY);

    int occ_counter = 0;
    int lines_counter = 0;
    int new_line;

    char c;
    while (read(file, &c, 1) == 1){
        if (c == '\n') {
            printf("newline\n");
        }
    }

    int* result = malloc(2* sizeof(int));
    result[0] = occ_counter;
    result[1] = lines_counter;
    return result;
}



int main(int argc, char** argv) {

    if (argc < 3) {
        printf("M8 you have to add at least two parameters, %d were given\n", argc);
        exit(1);
    }

    char target = argv[1][0];
    char* path = argv[2];
    int * result;

    result = sys_count(target, path);


    return 0;
}


