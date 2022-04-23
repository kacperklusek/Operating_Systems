#include <stdio.h>
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>


static int server_queue_id;
int active = 0;
int finished = 0;
ClientData clients[MAX_NO_CLIENTS];
char * curr_time;
time_t now;


void get_time() {
    struct tm * timeinfo;
    time (&now);
    timeinfo = localtime(&now);
    strcpy(curr_time, asctime(timeinfo));
    curr_time[24] = ' ';
}

void log_to_file(Message* msg) {
    FILE* log = fopen("log.txt", "a+");
    char line[100];
    get_time();
    snprintf(line, 100, "%s | client_index: %d | %s",
             curr_time, msg->index, TaskTypeStr[msg->type]);
    fwrite(line, strlen(line), sizeof (char), log);
    fputc('\n', log);
    fclose(log);
}

int get_free_index() {
    for (int i = 0; i < 128; ++i) {
        if (clients[i].status == NOT_CONNECTED){
            return i;
        }
    }
    return -1;
}

void end() {
    finished = 1;

    msgctl(server_queue_id, IPC_RMID, NULL);

    _Exit(EXIT_SUCCESS);
}

void handle_message(Message* message) {
    int client_idx = message->index;
    log_to_file(message);

    switch (message->type) {
        case INIT: {
            int msgid;
            if ((msgid = msgget((key_t) client_idx, PERMISSIONS)) == -1) {
                perror("msgget INIT");
                return;
            }

            Message m = {.type = INIT};
            int free_client_index = get_free_index();

            if (free_client_index == -1) { // no available places
                sprintf(&m.text[0], "%d", -1);
                if (msgsnd(msgid, &m, MESSAGE_SIZE, 0) == -1){
                    perror("msgsnd INIT");
                    return;
                }
                return;
            }

            clients[free_client_index].index = free_client_index;
            clients[free_client_index].status = CONNECTED;
            clients[free_client_index].que_id = msgid;
            clients[free_client_index].pid = atoi(message->text);
            m.index = clients[free_client_index].index;
            active++;

            if(msgsnd(clients[free_client_index].que_id,
                      &(Message) {.type = INIT, .index = clients[free_client_index].index, .text[0] = '\0'},
                      MESSAGE_SIZE, 0) == -1) {
                perror("msgsnd INIT");
                return;
            }

            printf("INIT | client_index: %d, que_id: %d\n",
                   clients[free_client_index].index,
                   clients[free_client_index].que_id);
            break;
        }
        case LIST: {
            printf("LIST | client_index: %d, que_id: %d\n",
                   clients[client_idx].index,
                   clients[client_idx].que_id);

            Message m = {.type = LIST, .index = client_idx};
            int len = 0;
            len += snprintf(&m.text[len], MAX_MSG_LEN, "You\t%d\n", clients[client_idx].index);
            for (int i = 0; i < MAX_NO_CLIENTS; ++i) {
                if (clients[i].status == CONNECTED && client_idx != i) {
                    len += snprintf(&m.text[len], MAX_MSG_LEN - len, " -\t%d\n", clients[i].index);
                }
            }

            if (msgsnd(clients[client_idx].que_id, &m, MESSAGE_SIZE, 0) == -1) {
                perror("msgsnd LIST");
                return;
            }
            break;
        }
        case TOALL: {
            printf("TOALL | client_index: %d, que_id: %d\n",
                   clients[client_idx].index,
                   clients[client_idx].que_id);
            Message m = {.type = TOALL, .index = client_idx};
            get_time();
            snprintf(m.text, MAX_MSG_LEN, "(from %d on %s): \t %s",
                     client_idx, curr_time, message->text);

            for (int i = 0; i < MAX_NO_CLIENTS; ++i) {
                if (clients[i].status == CONNECTED && i != client_idx) {
                    if(msgsnd(clients[i].que_id, &m, MESSAGE_SIZE, 0) == -1) {
                        perror("msgsnd TOALL");
                        return;
                    }
                }
            }
            break;
        }
        case TOONE: {
            printf("TOONE | client_index: %d, que_id: %d\n",
                   clients[client_idx].index,
                   clients[client_idx].que_id);
            Message m = {.type = TOONE, .index = client_idx};
            int target_index = message->to_index;
            get_time();
            snprintf(&m.text[0], MAX_MSG_LEN, "(from %d on %s): \t %s",
                     client_idx, curr_time, message->text);

            if (clients[target_index].status == NOT_CONNECTED) {
                perror("target client not CONNECTED");
                return;
            }
            if(msgsnd(clients[target_index].que_id, &m, MESSAGE_SIZE, 0) == -1) {
                perror("msgsnd TOONE");
                return;
            }
            break;
        }
        case STOP: {
            printf("STOP | client_index: %d\n", clients[client_idx].index);

            clients[client_idx].index = -1;
            clients[client_idx].status = NOT_CONNECTED;
            clients[client_idx].que_id = 0;
            clients[client_idx].pid = 0;

            if (--active == 0) {
                end();
            }
        }
        default: break;
    }
}

void listen() {
    Message message;
    memset(&clients[0], 0, MAX_NO_CLIENTS * sizeof (ClientData));

    while (finished == 0) {
        if (
                msgrcv(server_queue_id, &message, MESSAGE_SIZE, STOP, IPC_NOWAIT) == -1 &&
                msgrcv(server_queue_id, &message, MESSAGE_SIZE, LIST, IPC_NOWAIT) == -1 &&
                msgrcv(server_queue_id, &message, MESSAGE_SIZE, TOONE, IPC_NOWAIT) == -1 &&
                msgrcv(server_queue_id, &message, MESSAGE_SIZE, TOALL, IPC_NOWAIT) == -1 &&
                msgrcv(server_queue_id, &message, MESSAGE_SIZE, INIT, IPC_NOWAIT) == -1
                )
        {
            if (errno != ENOMSG){
                perror("msgrcv");
                exit(1);
            } else {
                sleep(1);
                continue;
            }
        }
        handle_message(&message);
    }

}

int main(int argc, char** argv) {
    signal(SIGINT, end);
    curr_time = calloc(24, sizeof (char));
    server_queue_id = create_msg_queue(SERVER_KEY_PATH, PROJECT_ID, IPC_CREAT);
    listen();

    return 1;
}

