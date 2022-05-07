#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

#define SEM_AVAILABLE_PLACES_OVEN "/oven"
#define SEM_AVAILABLE_PLACES_TABLE "/table"
#define SEM_TO_DELIVER "/deliveries"
#define SEM_MEMORY_LOCK "/memory"
#define SHARED_MEMORY_NAME "/sh_mem"
#define MAX_TASKS 10
#define PERMISSIONS 0660
#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5

enum {
    AVAILABLE_PLACES_OVEN = 0,
    AVAILABLE_PLACES_TABLE,
    TO_DELIVER,
    MEMORY_LOCK
};

//struct sembuf put_pizza_in_oven = {AVAILABLE_PLACES_OVEN, -1, 0};
//struct sembuf get_pizza_from_oven = {AVAILABLE_PLACES_OVEN, 1, 0};
//
//struct sembuf put_pizza_on_the_table = {AVAILABLE_PLACES_TABLE, -1, 0};
//struct sembuf get_pizza_from_table = {AVAILABLE_PLACES_TABLE, 1, 0};
//
//struct sembuf deliver_pizza = {TO_DELIVER, -1, 0};
//struct sembuf add_pizza_to_deliver = {TO_DELIVER, 1, 0};
//
//struct sembuf mem_lock = {MEMORY_LOCK, -1, 0};
//struct sembuf mem_unlock = {MEMORY_LOCK, 1, IPC_NOWAIT};

typedef struct {
    int o_pizzas_num;
    int o_start_index;
    int t_pizzas_num;
    int t_start_index;
    int oven[OVEN_CAPACITY];
    int table[TABLE_CAPACITY];
} pizzas_mem;


void get_current_time_with_ms(time_t *s, long *ms)
{
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    *s  = spec.tv_sec;
    *ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (*ms > 999) {
        (*s)++;
        *ms = 0;
    }
}

