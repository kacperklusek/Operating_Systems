#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define PRESENTS_TO_DELIVER 3
#define ELF_QUEUE_SIZE 3
#define N_OF_REINDEERS 9
int N_OF_ELFS;
#define ELF_WORK_TIME_FROM 2
#define ELF_WORK_TIME_TO 5
#define ELF_REPAIR_TIME_FROM 1
#define ELF_REPAIR_TIME_TO 2
#define REINDEER_VACATION_TIME_FROM 5
#define REINDEER_VACATION_TIME_TO 10
#define PRESENT_DELIVERY_FROM 2
#define PRESENT_DELIVERY_TO 4
#define PRESENT_DELIVERY_TO 4


pthread_mutex_t santa_shop_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wake_santa_up = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_left_queue = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_solved_elfs_problem = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_state_changed = PTHREAD_COND_INITIALIZER;

struct {
    int* elfs_queue;
    int elfs_in_queue;

    int reindeers_ready;

    int delivered_presents;
    bool santa_delivering_presents;
    bool santa_solving_elfs_problems;
    bool santa_sleeping;
    bool santa_done;
} santa_shop;



int get_random_time(int from, int to){
    return from + (rand() % (to - from));
}

int get_free_index(){
    for (int i = 0; i < ELF_QUEUE_SIZE; ++i) {
        if (santa_shop.elfs_queue[i] < 1){
            return i;
        }
    }
    return -1;
}

void* elf_worker(void *arg) {
    int *id = arg;
    int index;
    while (! santa_shop.santa_done) {
        sleep(get_random_time(ELF_WORK_TIME_FROM, ELF_WORK_TIME_TO));

        pthread_mutex_lock(&santa_shop_mutex);

        // wait for free place in queue
        while (santa_shop.elfs_in_queue == ELF_QUEUE_SIZE) {
            pthread_cond_wait(&santa_state_changed, &santa_shop_mutex);
        }

        santa_shop.elfs_in_queue ++;
        index = get_free_index();
        santa_shop.elfs_queue[index] = *id;

        if (santa_shop.elfs_in_queue == ELF_QUEUE_SIZE) {
            printf("Elf: %d elfy czekają, wołam do mikołaja, ID:%d\n", ELF_QUEUE_SIZE, *id);
            pthread_cond_signal(&wake_santa_up);
        } else {
            printf("Elf: czeka %d elfów na mikołaja, ID:%d\n", santa_shop.elfs_in_queue, *id);
        }

        // basically wait for entering santa shop
        // wait for santa to tell elfs to come in to the shop (in batches of three (ELF_QUEUE_SIZE)) or
        //                to be available (sleeping) and for the rest of the elfs
        while ( !santa_shop.santa_solving_elfs_problems ||
                ( santa_shop.santa_sleeping && santa_shop.elfs_in_queue < ELF_QUEUE_SIZE ) ) {
            pthread_cond_wait(&santa_state_changed, &santa_shop_mutex);
        }
        pthread_mutex_unlock(&santa_shop_mutex);

        // wait for end of the repairs
        pthread_mutex_lock(&santa_shop_mutex);
        while ( santa_shop.santa_solving_elfs_problems ) {
            pthread_cond_wait(&santa_solved_elfs_problem, &santa_shop_mutex);
        }
        santa_shop.elfs_queue[index] = 0;
        santa_shop.elfs_in_queue--;
        pthread_cond_broadcast(&elf_left_queue);
        pthread_mutex_unlock(&santa_shop_mutex);

        sleep(get_random_time(PRESENT_DELIVERY_FROM, PRESENT_DELIVERY_TO));
    }
    return id;
}

void* reindeer_worker(void* arg) {
    int *id = arg;
    while (! santa_shop.santa_done) {
        sleep(get_random_time(REINDEER_VACATION_TIME_FROM, REINDEER_VACATION_TIME_TO));

        pthread_mutex_lock(&santa_shop_mutex);
        santa_shop.reindeers_ready++;

        if (santa_shop.reindeers_ready == N_OF_REINDEERS) {
            printf("Renifer: wszystkie renifery gotowe, wołam do mikołaja, ID:%d\n", *id);
            pthread_cond_signal(&wake_santa_up);
        } else {
            printf("Renifer: czeka %d reniferów na mikołaja, ID:%d\n", santa_shop.reindeers_ready, *id);
        }

        // wait for santa to tell reindeers to go (deliver presents) or
        //                to be available (sleeping) and for the rest of the reindeers
        while ( !santa_shop.santa_delivering_presents ||
                ( santa_shop.santa_sleeping && santa_shop.reindeers_ready < N_OF_REINDEERS ) ) {
            pthread_cond_wait(&santa_state_changed, &santa_shop_mutex);
        }
        santa_shop.reindeers_ready--;
        pthread_mutex_unlock(&santa_shop_mutex);

        sleep(get_random_time(PRESENT_DELIVERY_FROM, PRESENT_DELIVERY_TO));
    }
    return id;
}

void* santa_worker(void* arg) {
    while (santa_shop.delivered_presents < PRESENTS_TO_DELIVER) {
        pthread_mutex_lock(&santa_shop_mutex);

        while (santa_shop.reindeers_ready < N_OF_REINDEERS && santa_shop.elfs_in_queue < ELF_QUEUE_SIZE) {
            pthread_cond_wait(&wake_santa_up, &santa_shop_mutex);
        }
        if (santa_shop.reindeers_ready == N_OF_REINDEERS) {
            santa_shop.santa_sleeping = false;
            santa_shop.santa_delivering_presents = true;
            pthread_cond_broadcast(&santa_state_changed);

            // zaczyna dostarczać
            santa_shop.delivered_presents++;
            santa_shop.santa_done = santa_shop.delivered_presents == PRESENTS_TO_DELIVER;

            printf("Mikołaj: dostarczam %d prezent\n", santa_shop.delivered_presents);
            pthread_mutex_unlock(&santa_shop_mutex);

            sleep(get_random_time(PRESENT_DELIVERY_FROM, PRESENT_DELIVERY_TO));

            pthread_mutex_lock(&santa_shop_mutex);
            // kończy dostarczać
            santa_shop.santa_delivering_presents = false;
        } else {
            santa_shop.santa_sleeping = false;
            santa_shop.santa_solving_elfs_problems = true;
            pthread_cond_broadcast(&santa_state_changed);

            //zaczyna rozwiązywać problemy
            printf("Mikołaj: rozwiązauje problemy eflów ");
            for (int i = 0; i < ELF_QUEUE_SIZE; ++i) {
                printf(" %d ", santa_shop.elfs_queue[i]);
            }
            printf("\n");
            pthread_mutex_unlock(&santa_shop_mutex);

            sleep(get_random_time(ELF_REPAIR_TIME_FROM, ELF_REPAIR_TIME_TO));

            pthread_mutex_lock(&santa_shop_mutex);
            // kończy rozwiązywać problemy elfów
            santa_shop.santa_solving_elfs_problems = false;
            pthread_cond_broadcast(&santa_solved_elfs_problem);

            while (santa_shop.elfs_in_queue > 0) {
                pthread_cond_wait(&elf_left_queue, &santa_shop_mutex);
            }
        }
        santa_shop.santa_sleeping = true;
        pthread_cond_broadcast(&santa_state_changed);
        pthread_mutex_unlock(&santa_shop_mutex);

    }
    return 0;
}

int main(int argc, char** argv){

    if (argc < 2) {
        puts("pass in number of elfs kolego złoty");
        exit(EXIT_FAILURE);
    }

    N_OF_ELFS = atoi(argv[1]);

    pthread_t santa;
    pthread_create(&santa, NULL, santa_worker, NULL);

    pthread_t *reindeers = calloc(N_OF_REINDEERS, sizeof (pthread_t));
    int *tids_reindeers = calloc(N_OF_REINDEERS, sizeof(int));
    santa_shop.reindeers_ready = 0;
    pthread_t *elfs = calloc(N_OF_ELFS, sizeof (pthread_t));
    int *tids_elfs = calloc(N_OF_ELFS, sizeof(int));
    santa_shop.elfs_queue = calloc(ELF_QUEUE_SIZE, sizeof (int));
    santa_shop.elfs_in_queue = 0;

    santa_shop.delivered_presents = 0;
    santa_shop.santa_sleeping=true;
    santa_shop.santa_done=false;
    santa_shop.santa_delivering_presents = false;
    santa_shop.santa_solving_elfs_problems = false;


    for (int i = 0; i < N_OF_REINDEERS; ++i) {
        tids_reindeers[i] = i+1;
        pthread_create(reindeers + i, NULL, reindeer_worker, tids_reindeers + i);
    }
    for (int i = 0; i < N_OF_ELFS; ++i) {
        tids_elfs[i] = i+1;
        pthread_create(elfs + i, NULL, elf_worker, tids_elfs + i);
    }

    pthread_join(santa, NULL);
    for (int i = 0; i < N_OF_REINDEERS; ++i) {
        pthread_join(reindeers[i], NULL);
    }

    free(reindeers);
    free(elfs);
    free(tids_elfs);
    free(tids_reindeers);
    free(santa_shop.elfs_queue);

    return 0;
}
