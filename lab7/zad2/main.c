#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>

#include "common.h"

sem_t *semaphores[4];
int shared_memory_id;
int workers_num;
pid_t *children;

void create_semaphore(){
    sem_unlink(SEM_AVAILABLE_PLACES_OVEN);
    sem_unlink(SEM_AVAILABLE_PLACES_TABLE);
    sem_unlink(SEM_TO_DELIVER);
    sem_unlink(SEM_MEMORY_LOCK);

    semaphores[AVAILABLE_PLACES_OVEN] = sem_open(SEM_AVAILABLE_PLACES_OVEN, O_CREAT, PERMISSIONS, OVEN_CAPACITY);
    semaphores[AVAILABLE_PLACES_TABLE] = sem_open(SEM_AVAILABLE_PLACES_TABLE, O_CREAT, PERMISSIONS, TABLE_CAPACITY);
    semaphores[TO_DELIVER] = sem_open(SEM_TO_DELIVER, O_CREAT, PERMISSIONS, 0);
    semaphores[MEMORY_LOCK] = sem_open(SEM_MEMORY_LOCK, O_CREAT, PERMISSIONS, 1);

    for (int i = 0; i < 4; ++i) {
        sem_close(semaphores[i]);
    }
}

void create_shared_memory(){
    shared_memory_id = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, PERMISSIONS);
    ftruncate(shared_memory_id, sizeof (pizzas_mem));

    pizzas_mem *mem = mmap(
            NULL,
            sizeof (pizzas_mem),
            PROT_WRITE|PROT_READ,
            MAP_SHARED,
            shared_memory_id,
            0);
    memset(mem, 0, sizeof (pizzas_mem));
    munmap(mem, sizeof (pizzas_mem));
}

void cleanup(){
    puts("cleanup");
    if (shared_memory_id != -1) {
        shm_unlink(SHARED_MEMORY_NAME);
        shared_memory_id = -1;
    }

    sem_unlink(SEM_AVAILABLE_PLACES_OVEN);
    sem_unlink(SEM_AVAILABLE_PLACES_TABLE);
    sem_unlink(SEM_TO_DELIVER);
    sem_unlink(SEM_MEMORY_LOCK);

    free(children);
    for (int i = 0; i < 4; ++i) {
        semaphores[i] = SEM_FAILED;
    }
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
    atexit(cleanup);

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

    return 0;
}

