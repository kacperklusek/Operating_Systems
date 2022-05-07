#include "common.h"

sem_t *semaphores[4];
pizzas_mem *smem;
int running = 1;

void open_resources(){
    semaphores[AVAILABLE_PLACES_OVEN] = sem_open(SEM_AVAILABLE_PLACES_OVEN, O_RDWR);
    semaphores[AVAILABLE_PLACES_TABLE] = sem_open(SEM_AVAILABLE_PLACES_TABLE, O_RDWR);
    semaphores[TO_DELIVER] = sem_open(SEM_TO_DELIVER, O_RDWR);
    semaphores[MEMORY_LOCK] = sem_open(SEM_MEMORY_LOCK, O_RDWR);

    for (int i = 0; i < 4; ++i) {
        if (semaphores[i] == SEM_FAILED) {
            perror("sem_open");
        }
    }
    int sh_mem_id = shm_open(SHARED_MEMORY_NAME, O_RDWR, PERMISSIONS);
    smem = mmap(
            NULL,
            sizeof (pizzas_mem),
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            sh_mem_id,
            0);
}

void close_semaphores(){
    running = 0;
}

int main(int argc, char ** argv){
    srand(time(NULL) + getpid());
    signal(SIGINT, close_semaphores);
    open_resources();

    long ms;
    time_t s;
    int pizza_type;
    int p_index;
    while(running == 1){
        pizza_type = rand() % 10;
        get_current_time_with_ms(&s, &ms);
        printf("(%d %d.%03ld) Przygotowuję pizze: %d\n",
               getpid(), (int) s, ms, pizza_type);
        sleep(1); // sleep 1 or 2 seconds

        sem_wait(semaphores[AVAILABLE_PLACES_OVEN]);
        sem_wait(semaphores[MEMORY_LOCK]);
        p_index = (smem->o_start_index + smem->o_pizzas_num) % OVEN_CAPACITY;

        smem->oven[p_index] = pizza_type;
        smem->o_pizzas_num++;
        get_current_time_with_ms(&s, &ms);
        printf("(%d %d.%03ld) Dodałem pizze: %d. Liczba pizz w piecu: %d\n",
               getpid(), (int) s, ms, pizza_type,
               smem->o_pizzas_num);
        sem_post(semaphores[MEMORY_LOCK]);

        sleep(4); // sleep 4 or 5 seconds

        sem_wait(semaphores[AVAILABLE_PLACES_TABLE]);
        sem_wait(semaphores[MEMORY_LOCK]);
        sem_post(semaphores[AVAILABLE_PLACES_OVEN]);
        sem_post(semaphores[TO_DELIVER]);

        smem->oven[p_index] = 10; // set free
        smem->o_start_index++;
        smem->o_pizzas_num--;
        smem->table[(smem->t_start_index + smem->t_pizzas_num) % OVEN_CAPACITY] = pizza_type;
        smem->t_pizzas_num++;
        get_current_time_with_ms(&s, &ms);
        printf("(%d %d.%03ld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n",
               getpid(), (int) s, ms, pizza_type,
               smem->o_pizzas_num,
               smem->t_pizzas_num);

        sem_post(semaphores[MEMORY_LOCK]);
    }

    for (int i = 0; i < 4; ++i) {
        sem_close(semaphores[i]);
    }

    munmap(smem, sizeof (pizza_type));

    return 0;
}