#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define SEMAPHORE_PROJECT_ID 1234
#define MEMORY_PROJECT_ID 1235
#define MAX_TASKS 10
#define PERMISSIONS 0666
#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5

enum {
    AVAILABLE_PLACES_OVEN = 0,
    AVAILABLE_PLACES_TABLE,
    TO_DELIVER,
    MEMORY_LOCK
};

struct sembuf put_pizza_in_oven = {AVAILABLE_PLACES_OVEN, -1, 0};
struct sembuf get_pizza_from_oven = {AVAILABLE_PLACES_OVEN, 1, 0};

struct sembuf put_pizza_on_the_table = {AVAILABLE_PLACES_TABLE, -1, 0};
struct sembuf get_pizza_from_table = {AVAILABLE_PLACES_TABLE, 1, 0};

struct sembuf deliver_pizza = {TO_DELIVER, -1, 0};
struct sembuf add_pizza_to_deliver = {TO_DELIVER, 1, 0};

struct sembuf mem_lock = {MEMORY_LOCK, -1, 0};
struct sembuf mem_unlock = {MEMORY_LOCK, 1, IPC_NOWAIT};

//typedef enum {
//    NONE = 0,
//    BAKED,
//    ON_TABLE
//} PizzaStatus;

//typedef struct {
//    int pizza_type;
////    PizzaStatus status;
//} Pizza;

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

