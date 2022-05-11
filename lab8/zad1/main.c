#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include <pthread.h>
#include <math.h>

#define MAX_LINE_LEN 80
#define MAX_COLORS 256

int NEW_MAX_COLOR = 0;
int n_of_threads;

struct {
    int height;
    int width;
    unsigned char **data;
} IMAGE;

void read_until_valid_line(char *buffer, FILE *file) {
    do {
        if (fgets(buffer, MAX_LINE_LEN, file) == NULL){
            buffer[0] = '\0';
            return;
        }
    } while (buffer[0] == '\n' || buffer[0] == '#'); // just omit comments and empty lines
}

void load_image(const char * filename){
    const char* delims = " \n\t";
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    char buff[MAX_LINE_LEN];

    read_until_valid_line(buff, f); // reads P2 ot buff
    read_until_valid_line(buff, f); // omits comment and reads W H

    sscanf(buff, "%d %d", &IMAGE.width, &IMAGE.height);
    IMAGE.data = calloc(IMAGE.height, sizeof (unsigned char*));
    for (int i = 0; i < IMAGE.height; ++i) {
        IMAGE.data[i] = calloc(IMAGE.width, sizeof (unsigned char));
    }

    read_until_valid_line(buff, f); // skip reading max gray value

    char *line;
    int chr;
    for (int i = 0; i < IMAGE.height; ++i) {
        for (int j = 0; j < IMAGE.width; ++j) {
            fscanf(f, " %d", &chr);
            IMAGE.data[i][j] = (char) chr;
        }
    }

    fclose(f);
}

long get_time(struct timespec *s) {
    struct timespec e;
    clock_gettime(CLOCK_MONOTONIC, &e);
    long retval = (e.tv_sec - s->tv_sec) * 1000000;
    retval += (e.tv_nsec - s->tv_nsec) / 1000.0;
    return retval;
}

long divide_values_mode(int *thread_num){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);



    return get_time(&start);
}

long block_mode(int* thread_num){
    int k = *thread_num;
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i =0; i < IMAGE.height; ++i) {
        for (int j = k * (int) ceil(IMAGE.width / n_of_threads);
             j < (k+1) * (int) ceil(IMAGE.width / n_of_threads);
                ++j) {
//            printf("num: %d, i: %d,  j: %d\n", k, i, j);
            IMAGE.data[i][j] = 512 - IMAGE.data[i][j];
            if (IMAGE.data[i][j] > NEW_MAX_COLOR){
                NEW_MAX_COLOR = IMAGE.data[i][j];
            }
        }
    }

    return get_time(&start);
}

void save_image(const char *outfile) {
    FILE *f = fopen(outfile, "w+");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fputs("P2\n", f);
    fprintf(f, "%d %d\n", IMAGE.width, IMAGE.height);
    fprintf(f, "%d\n", NEW_MAX_COLOR);

    for (int i = 0; i < IMAGE.height; ++i) {
        for (int j = 0; j < IMAGE.width; ++j) {
            fprintf(f, " %d", IMAGE.data[i][j]);
        }
        fputc('\n', f);
    }

}

int main(int argc, char** argv){
    if (argc < 5) {
        puts("at least 5 arguments big man");
        exit(EXIT_FAILURE);
    }

    n_of_threads = atoi(argv[1]);
    int threading_mode = atoi(argv[2]);
    char* filename = argv[3];
    char* out_filename = argv[4];

    load_image(filename);

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);


    pthread_t *threads = calloc(n_of_threads, sizeof (pthread_t));
    int *tids = calloc(n_of_threads, sizeof(int));

    printf("BEFORE %d\n", IMAGE.data[497][128]);

    for (int i = 0; i < n_of_threads; ++i) {
        tids[i] = i;
        if (threading_mode == 1){
            pthread_create(threads + i, NULL, (void*) block_mode, (void*) (tids + i));
        } else {
            pthread_create(threads + i, NULL, (void*) divide_values_mode, (void*) (tids + i));
        }
    }

    long ret_time;
    for (int i = 0; i < n_of_threads; ++i) {
        pthread_join(threads[i], (void*) &ret_time);
        printf("Thread %lu returned %ld microseconds\n", threads[i], ret_time);
    }

    printf("Main thread took %ld microseconds\n", get_time(&start));

    save_image(out_filename);

    free(threads);

    return 0;
}
