#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "stdio.h"

#ifdef LIB_DYNAMIC
    #include <dlfcn.h>
#else
    #include "library.c"
#endif

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



int main(int argc, char **argv) {

    #ifdef LIB_DYNAMIC
        void *handle = dlopen("./library.so", RTLD_LAZY);
        if (!handle) { puts("Cannot load LIB_DYNAMIC library.so\n"); exit(1);}

        blocks_table (*create_table)(int) = (blocks_table (*)(int)) dlsym(handle, "create_table");
        FILE* (*wc_files)(char*) = (FILE* (*)(char*)) dlsym(handle, "wc_files");
        int (*save_block)(FILE*, blocks_table*) = (int (*)(FILE*, blocks_table*)) dlsym(handle, "save_block");
        void (*remove_block)(blocks_table*, int) = (void (*)(blocks_table*, int)) dlsym(handle, "remove_block");
    #endif


    blocks_table table;
    int table_size;
    FILE* file;

    if (argc <= 1) {
        puts("Too few arguments");
        exit(1);
    }

    regex_t re;
    int ret = regcomp(&re, "[/a-zA -Z0-9._-]+[.][a-zA-Z0-9._-]+", REG_EXTENDED);
    if (ret != 0) {
        char errmsg[256];
        regerror(ret, &re, errmsg, sizeof(errmsg));
        printf("Compiling regex error | %s\n", errmsg);
        exit(1);
    }

    //    DO NOT REMOVE, IF REMOVED SEGFAULT XDDDDD
    char* arguments[argc];
    for (int i = 1; i < argc; ++i) {
        arguments[i] = argv[i];
    }
    //    DO NOT REMOVE, IF REMOVED SEGFAULT XDDDDD

    char* cmd;
    int i = 1;
    for (; i < argc; i++) {
        cmd = argv[i];

        if (strcmp(cmd, "create_table") == 0) {
            // need to have table size
            if (argc <= i+1) {
                puts("create_table expects one argument");
                exit(1);
            }
            table_size = atoi(argv[++i]);
            table = create_table(table_size);
        }
        else if (strcmp(cmd, "wc_files") == 0) {
            // needs at least one file
            if (argc <= i+1) {
                puts("wc_files expects at least one argument");
                exit(1);
            }

            int start = ++i;
            int count = 0;
            for (; i < argc; i++) {
                if (regexec(&re, argv[i], 0, NULL, 0) == 0){
                    count += strlen(argv[i]);
                } else {
                    // argument is not a file name
                    i--;
                    break;
                }
            }

            char paths[count + i - start + 1];
            for (; start <= i; start++) {
                strcat(paths, argv[start]);
                strcat(paths, " ");
            }
            printf("%s", paths);

            file = wc_files(paths);
        }
        else if (strcmp(cmd, "save_block") == 0) {
            if (!file) {
                puts("save blocks expects tmpfile to be present");
                exit(1);
            }
            save_block(file, &table);
        }
        else if (strcmp(cmd, "remove_block") == 0) {
            if (argc <= i+1) {
                puts("remove_block expects one argument");
                exit(1);
            }
            int index = atoi(argv[++i]);
            if (index >= table_size || index < 0) {
                puts("wrong index");
                exit(1);
            }
            remove_block(&table, index);
        }
        #ifdef TIMING
        else if (strcmp(cmd, "start_timer") == 0) {
            start_timer();
        }
        else if (strcmp(cmd, "stop_timer") == 0) {
            if (argc <= i+1) {
                puts("stop_timer expects one argument");
            }
            stop_timer();
            print_times(argv[++i]);
        }
        #endif
        else {
            printf("Unknown argument: %s", argv[i]);
            exit(1);
        }


    }

    return 0;
}


