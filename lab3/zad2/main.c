#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>

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

double f(double x) {
    return 4/(x*x + 1);
}

int main(int argc, char** argv) {
    int FROM=0;
    int TO=1;
    double REC_W = atof(argv[1]);

    #ifdef TIMING
    start_timer();
    #endif

    for (double i = FROM; i < TO;) {
        if (fork() == 0) {
            double result = f(i + REC_W/2) * REC_W;
            char filename[50];
            snprintf(filename, 50, "./res/w%d.txt", getpid() - getppid());
            FILE* res_file = fopen(filename, "w+");
            fprintf(res_file, "%f", result);
            fclose(res_file);
            exit(0);
        }
        i += REC_W;
    }
    wait(0);

    double solution = 0;

    FILE*res_file;
    double tmp_res;
    char filename[50];
    char line[10];
    int i = 1;
    for (double j = FROM; j < TO;i++) {
        snprintf(filename, 50, "./res/w%d.txt", i);
        res_file = fopen(filename, "r+");
        fgets(line, sizeof line, res_file);
        fclose(res_file);
        tmp_res = atof(line);
        solution += tmp_res;

        j += REC_W;
    }

    #ifdef TIMING
    stop_timer();
    print_times("calculation times:");
    #endif

    printf("result: %f\n", solution);

    return 0;
}

