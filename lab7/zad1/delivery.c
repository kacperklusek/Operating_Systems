#include "common.h"

int main(int argc, char ** argv){
    srand(time(NULL) + getpid());
    key_t sem_key = ftok(getenv("HOME"), SEMAPHORE_PROJECT_ID);
    int semaphore_id = semget(sem_key, 0, PERMISSIONS);

    sem_key = ftok(getenv("HOME"), MEMORY_PROJECT_ID);
    int sh_memory_id = shmget(sem_key, 0, PERMISSIONS);

    long ms;
    time_t s;
    int pizza_type;
    int p_index;
    while(1 == 1){
        int uloc = 0;
        pizzas_mem *smem = shmat(sh_memory_id, NULL, 0);

        semop(semaphore_id, (struct sembuf[3]) {deliver_pizza, get_pizza_from_table ,mem_lock},
                3);
        for (int i = smem->t_start_index; i < smem->t_start_index + smem->t_pizzas_num; ++i) {
            p_index = i % TABLE_CAPACITY;
            if (smem->table[p_index] > 9 || smem->table[p_index] < 0) {
                printf("Error!!\n");
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
            semop(semaphore_id, (struct sembuf[1]) {mem_unlock}, 1);

            sleep( 4);

            get_current_time_with_ms(&s, &ms);
            printf("(%d %d.%03ld) Dostarczam pizze: %d\n",
                   getpid(), (int) s, ms, pizza_type);

            sleep(4);
            printf("Wróciłem\n");
            break; // gotta take one by one to ensure sync via semaphores
        }
        if (uloc == 0) {
            semop(semaphore_id, (struct sembuf[1]) {mem_unlock}, 1);
        }
        shmdt(smem);
    }
}