#include "common.h"

sem_t *semaphores[4];
pizzas_mem *smem;
int running = 1;

void open_resources(){
    semaphores[AVAILABLE_PLACES_OVEN] = sem_open(SEM_AVAILABLE_PLACES_OVEN, O_RDWR);
    semaphores[AVAILABLE_PLACES_TABLE] = sem_open(SEM_AVAILABLE_PLACES_TABLE, O_RDWR);
    semaphores[TO_DELIVER] = sem_open(SEM_TO_DELIVER, O_RDWR);
    semaphores[MEMORY_LOCK] = sem_open(SEM_MEMORY_LOCK, O_RDWR);
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
    open_resources();
    signal(SIGINT, close_semaphores);

    long ms;
    time_t s;
    int pizza_type;
    int p_index;
    while(running == 1){
        int uloc = 0;

        sem_wait(semaphores[TO_DELIVER]);
        sem_wait(semaphores[MEMORY_LOCK]);
        sem_post(semaphores[AVAILABLE_PLACES_TABLE]);
        for (int i = smem->t_start_index; i < smem->t_start_index + smem->t_pizzas_num; ++i) {
            p_index = i % TABLE_CAPACITY;
            if (smem->table[p_index] > 9 || smem->table[p_index] < 0) {
                printf("ERROR");
                continue;
            }

            pizza_type = smem->table[p_index];
            smem->table[p_index] = 10;
            smem->t_pizzas_num--;
            smem->t_start_index++;
            uloc = 1;
            get_current_time_with_ms(&s, &ms);
            printf("(%d %d.%03ld) Pobieram pizze: %d. Liczba pizz na stole: %d\n",
                   getpid(), (int) s, ms, pizza_type,
                   smem->t_pizzas_num);
            sem_post(semaphores[MEMORY_LOCK]);

            sleep( 4);

            get_current_time_with_ms(&s, &ms);
            printf("(%d %d.%03ld) Dostarczam pizze: %d\n",
                   getpid(), (int) s, ms, pizza_type);

            sleep(4);
            printf("Wróciłem\n");
            break; // gotta take one by one to ensure sync via semaphores
        }
        if (uloc == 0) {
            sem_post(semaphores[MEMORY_LOCK]);
        }
    }

    for (int i = 0; i < 4; ++i) {
        sem_close(semaphores[i]);
    }

    munmap(smem, sizeof (pizza_type));

    return 0;
}