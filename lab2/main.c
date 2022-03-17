#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "stdlib.h"



int* sys_count(char target, char*path) {
    int file = open(path, O_RDONLY);

    int occ_counter = 0;
    int lines_counter = 0;
    int new_line = 0;

    char c;
    while (read(file, &c, 1) == 1){
        if (isspace(c) != 0 && c != ' ' && c != '\t') {
            new_line = 0;
        }
        if (c == target) {
            occ_counter++;
            if(new_line == 0) {
                lines_counter++;
                new_line = 1;
            }
        }
    }

    int* result = malloc(2* sizeof(int));
    result[0] = occ_counter;
    result[1] = lines_counter;

    close(file);
    return result;
}


int* lib_count(char target, char*path) {
    FILE* file = fopen(path, "r");

    int occ_counter = 0;
    int lines_counter = 0;
    int new_line = 0;

    char c;
    while (fread(&c, sizeof (char), 1, file) == 1){
        if (isspace(c) != 0 && c != ' ' && c != '\t') {
            new_line = 0;
        }
        if (c == target) {
            occ_counter++;
            if(new_line == 0) {
                lines_counter++;
                new_line = 1;
            }
        }
    }

    int* result = malloc(2* sizeof(int));
    result[0] = occ_counter;
    result[1] = lines_counter;

    fclose(file);
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

    printf("%d occurences in %d lines\n", result[0], result[1]);

    result = lib_count(target, path);

    printf("%d occurences in %d lines\n", result[0], result[1]);

    return 0;
}