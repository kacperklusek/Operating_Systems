#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define PRESENTS_TO_DELIVER 3
#define ELF_FIX_NUMBER 3
#define N_OF_REINDEERS 9
#define REINDEER_VACATION_TIME_FROM 5
#define REINDEER_VACATION_TIME_TO 10
#define PRESENT_DELIVERY_FROM 2
#define PRESENT_DELIVERY_TO 4
#define PRESENT_DELIVERY_TO 4

pthread_mutex_t reindeers_station = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_in_station = PTHREAD_COND_INITIALIZER; // zmienić na all reindeers ready
pthread_cond_t santa_went = PTHREAD_COND_INITIALIZER; // zmienić na all reindeers ready
int reindeers_ready = 0;
bool santa_delivering_presents = false;
int delivered_presents = 0;
bool santa_done;


int get_random_time(int from, int to){
    return from + (rand() % (to - from));
}

void* reindeer_worker(void* arg) {
    int *id = arg;
    while (! santa_done) {
        sleep(get_random_time(REINDEER_VACATION_TIME_FROM, REINDEER_VACATION_TIME_TO));

        pthread_mutex_lock(&reindeers_station);
        reindeers_ready += 1;

        if (reindeers_ready == 9) {
            printf("Renifer: wybudzam mikołaja, ID:%d\n", *id);
            pthread_cond_signal(&reindeer_in_station);
        } else {
            printf("Renifer: czeka %d reniferów na mikołaja, ID:%d\n", reindeers_ready, *id);
        }

        // wait for all reindeers
        while (!santa_delivering_presents) {
            pthread_cond_wait(&santa_went, &reindeers_station);
        }

        reindeers_ready -= 1;

        pthread_mutex_unlock(&reindeers_station);

        sleep(get_random_time(PRESENT_DELIVERY_FROM, PRESENT_DELIVERY_TO));
    }
    return id;
}

void* santa_worker(void* arg) {
    while (delivered_presents < PRESENTS_TO_DELIVER) {
        pthread_mutex_lock(&reindeers_station);

        while (reindeers_ready < N_OF_REINDEERS) {
            pthread_cond_wait(&reindeer_in_station, &reindeers_station);
        }
        // zaczyna dostarczać
        santa_delivering_presents = true;
        delivered_presents += 1;
        santa_done = delivered_presents == PRESENTS_TO_DELIVER;

        pthread_cond_broadcast(&santa_went);
        printf("Mikołaj: dostarczam %d prezent\n", delivered_presents);
        pthread_mutex_unlock(&reindeers_station);

        sleep(get_random_time(PRESENT_DELIVERY_FROM, PRESENT_DELIVERY_TO));

        pthread_mutex_lock(&reindeers_station);
        // kończy dostarczać
        santa_delivering_presents = false;
        pthread_mutex_unlock(&reindeers_station);
    }
    return 0;
}

int main(int argc, char** argv){

    pthread_t santa;
    pthread_create(&santa, NULL, santa_worker, NULL);

    pthread_t *reindeers = calloc(N_OF_REINDEERS, sizeof (pthread_t));
    int *tids = calloc(N_OF_REINDEERS, sizeof(int));
    for (int i = 0; i < N_OF_REINDEERS; ++i) {
        tids[i] = i+1;
        pthread_create(reindeers + i, NULL, reindeer_worker, tids + i);
    }

    pthread_join(santa, NULL);
    for (int i = 0; i < N_OF_REINDEERS; ++i) {
        pthread_join(reindeers[i], NULL);
    }

    free(reindeers);
    free(tids);

    return 0;
}
