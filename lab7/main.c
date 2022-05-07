#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <errno.h>

#include "common.h"

int semaphore_id;
int shared_memory_id;
int workers_num;
pid_t *children;

void create_semaphore(){
    key_t sem_key = ftok(getenv("HOME"), SEMAPHORE_PROJECT_ID);
    semaphore_id = semget(sem_key, 4, IPC_CREAT | PERMISSIONS);

    semctl(semaphore_id, AVAILABLE_PLACES_OVEN, SETVAL, OVEN_CAPACITY);
    semctl(semaphore_id, AVAILABLE_PLACES_TABLE, SETVAL, TABLE_CAPACITY);
    semctl(semaphore_id, TO_DELIVER, SETVAL, 0);
    semctl(semaphore_id, MEMORY_LOCK, SETVAL, 1);
}

void create_shared_memory(){
    // request a key
    key_t sm_key = ftok(getenv("HOME"), MEMORY_PROJECT_ID);
    // get shared block -- create if it doesn't exist
    shared_memory_id = shmget(sm_key, sizeof (pizzas_mem), PERMISSIONS | IPC_CREAT);
    // attach memory block to process' memory
    pizzas_mem *mem = shmat(shared_memory_id, NULL, 0);
    memset(mem, 0, sizeof (pizzas_mem));
    shmdt(mem);
}

void cleanup(){
    if(shared_memory_id != -1){
        shmctl(shared_memory_id, IPC_RMID, 0);
    }

    if(semaphore_id != -1){
        semctl(semaphore_id, 0, IPC_RMID);
    }

    free(children);
    shared_memory_id = -1;
    semaphore_id = -1;
    children = NULL;
}

void kill_children(){
    for(int i = 0; i < workers_num; i++){
        kill(children[i], SIGINT);
    }
    cleanup();
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        puts("gimme 2 args m8");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, kill_children);
    signal(SIGTERM, kill_children);

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    workers_num = N + M;

    create_semaphore();
    create_shared_memory();
    children = calloc(workers_num, sizeof(pid_t));
    int children_indexer = 0;
    pid_t pid;

    for (int i = 0; i < N; ++i) {
        if ((pid = fork()) == 0){
            execl("./chef", "./chef", NULL);
            exit(0);
        }
        children[children_indexer++] = pid;
    }
    for (int i = 0; i < M; ++i) {
        if ((pid = fork()) == 0){
            execl("./delivery", "./delivery", NULL);
            exit(0);
        }
        children[children_indexer++] = pid;
    }

    for (int i = 0; i < workers_num; ++i) {
        wait(0);
    }

    cleanup();
    return 0;
}

