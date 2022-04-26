#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "stdlib.h"


#ifdef TIMING
#include <unistd.h>
#include <sys/times.h>
clock_t clock_start, clock_stop;
struct tms times_start_buff, times_end_buff;

void start_timer() {
    clock_start = times(&times_start_buff);
}

void stop_timer() {
    clock_stop = times(&times_end_buff);
}

double calc_time(clock_t start, clock_t end) {
    return (double) (end - start) / (double) sysconf(_SC_CLK_TCK);
}

void print_times(char* op_name) {
    printf("%20s    real %.3fs  use r%.3fs    sys %.3fs\n",
           op_name,
           calc_time(clock_start, clock_stop),
           calc_time(times_start_buff.tms_utime, times_end_buff.tms_utime),
           calc_time(times_start_buff.tms_cstime, times_end_buff.tms_cstime));
}
#endif


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

//    char target = argv[1][0];
//    char* path = argv[2];
//    int * result;
//
//    result = sys_count(target, path);
//
//    printf("%d occurences in %d lines\n", result[0], result[1]);
//
//    result = lib_count(target, path);
//
//    printf("%d occurences in %d lines\n", result[0], result[1]);

    #ifdef TIMING
    start_timer();
    sys_count(argv[1][0], argv[2]);
    stop_timer();
    print_times("SYS count functions");

    start_timer();
    lib_count(argv[1][0], argv[2]);
    stop_timer();
    print_times("LIB count functions");

    #endif

    return 0;
}