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
        pizzas_mem *smem = shmat(sh_memory_id, NULL, 0);

        pizza_type = rand() % 10;
        get_current_time_with_ms(&s, &ms);
        printf("(%d %d.%03ld) Przygotowuję pizze: %d\n",
               getpid(), (int) s, ms, pizza_type);
        sleep(1); // sleep 1 or 2 seconds

        semop(semaphore_id, (struct sembuf[2]) {mem_lock, put_pizza_in_oven}, 2);
        p_index = (smem->o_start_index + smem->o_pizzas_num) % OVEN_CAPACITY;

        smem->oven[p_index] = pizza_type;
        smem->o_pizzas_num++;
        get_current_time_with_ms(&s, &ms);
        printf("(%d %d.%03ld) Dodałem pizze: %d. Liczba pizz w piecu: %d\n",
               getpid(), (int) s, ms, pizza_type,
               smem->o_pizzas_num);
        semop(semaphore_id, (struct sembuf[1]) {mem_unlock}, 1);

        sleep(4); // sleep 4 or 5 seconds

        semop(semaphore_id,
              (struct sembuf[4])
                      {get_pizza_from_oven, put_pizza_on_the_table, add_pizza_to_deliver, mem_lock},
              4);
        // wyjmuje z pieca
        smem->oven[p_index] = 10; // set free
        smem->o_start_index++;
        smem->o_pizzas_num--;
        // kłade na stole do wysyłki
        smem->table[(smem->t_start_index + smem->t_pizzas_num) % OVEN_CAPACITY] = pizza_type;
        smem->t_pizzas_num++;
        get_current_time_with_ms(&s, &ms);
        printf("(%d %d.%03ld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n",
               getpid(), (int) s, ms, pizza_type,
               smem->o_pizzas_num,
               smem->t_pizzas_num);

        semop(semaphore_id, (struct sembuf[1]) {mem_unlock}, 1);
        shmdt(smem);
    }
}