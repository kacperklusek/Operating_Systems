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

static int client_queue_id;
static int server_queue_id;
static int client_index;

void cleanup(){
    Message m = {.type=STOP, .index=client_index};
    if (msgsnd(server_queue_id, &m, MESSAGE_SIZE, 0) == -1) {
        perror("msgsnd error on cleanup");
        _Exit(EXIT_FAILURE);
    }
    msgctl(client_queue_id, IPC_RMID, NULL);
    puts("cleanup done... exiting");
    _Exit(EXIT_SUCCESS);
}

int init_connection_to_server(key_t key) {
    // here we do not create but retrieve the msgid
    server_queue_id = create_msg_queue(SERVER_KEY_PATH, PROJECT_ID, 0);
    Message message = {.type = INIT, .index=key};

    sprintf(&message.text[0], "%d", getpid());
    if (msgsnd(server_queue_id, &message, MESSAGE_SIZE, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    if (msgrcv(client_queue_id, &message, MESSAGE_SIZE, INIT, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    int ret = atoi(message.text);
    if (ret == -1) {
        printf("No free slots\n");
        _Exit(0);
    }

    return message.index;
}

void handle_mode(char* mode) {
    if (strcmp(mode, "LIST") == 0) {
        Message m = {.type = LIST, .index = client_index};

        if (msgsnd(server_queue_id, &m, MESSAGE_SIZE, 0) == -1) {
            perror("msgsnd LIST client");
            return;
        }

        if (msgrcv(client_queue_id, &m, MESSAGE_SIZE, LIST, 0) == -1) {
            perror("msgrcv LIST client");
            return;
        }

        puts("Reveived from server:");
        printf("%s\n", m.text);
        puts("");
    }
    else if (strcmp(mode, "TOALL") == 0) {
        printf("enter message: ");
        char text[MAX_MSG_LEN];
        scanf("%s", text);

        Message m = {.type=TOALL, .index=client_index};
        strcpy(m.text, text);

        if (msgsnd(server_queue_id, &m, MESSAGE_SIZE, 0) == -1) {
            perror("msgsnd TOALL client");
            return;
        }
        printf("sent %s, to id: %d\n", TaskTypeStr[m.type], server_queue_id);
    }
    else if (strcmp(mode, "TOONE") == 0) {
        printf("enter message: ");
        char text[MAX_MSG_LEN];
        scanf("%s", text);

        Message m = {.type=TOONE, .index=client_index};
        sprintf(m.text, "%s", text);

        printf("enter partner index: ");
        int idx;
        scanf("%d", &idx);
        m.to_index = idx;

        if (msgsnd(server_queue_id, &m, MESSAGE_SIZE, 0) == -1) {
            perror("msgsnd TOONE client");
            return;
        }
        printf("message '%s' sent to index: %d\n", m.text, m.to_index);
    }
}


void listen(key_t key) {
    client_index = init_connection_to_server(key);
    printf("My ID: %d\n", client_index);
    Message message;
    printf("choose what to do, (LIST / TOALL / TOONE / STOP / HELP / r to refresh ) \n");


    char mode[5];
    for (;;) {
        if (
                msgrcv(client_queue_id, &message, MESSAGE_SIZE, LIST, IPC_NOWAIT) == -1 &&
                msgrcv(client_queue_id, &message, MESSAGE_SIZE, TOONE, IPC_NOWAIT) == -1 &&
                msgrcv(client_queue_id, &message, MESSAGE_SIZE, TOALL, IPC_NOWAIT) == -1
                )
        {
            printf("> ");
            scanf("%s", mode);
            if (strcmp(mode, "HELP") == 0) {
                printf("choose what to do, (LIST / TOALL / TOONE / STOP / REFRESH ) \n");
            }
            else if (strcmp(mode, "r") == 0) {
                continue;
            }
            else if (strcmp(mode, "STOP") == 0) {
                break;
            }
            handle_mode(mode);
        } else {
            printf("Message %s\n\t %s\n", TaskTypeStr[message.type], message.text );
        }
    }
}

int main(int argc, char** argv) {
    atexit(cleanup);
    signal(SIGINT, cleanup);

    const char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        home_dir = argv[0];
    }

    client_queue_id = create_msg_queue(home_dir, getpid(), IPC_CREAT);

    key_t key;
    if ((key = ftok(home_dir, getpid())) == -1) {
        perror("ftok client");
        exit(EXIT_FAILURE);
    }

    listen(key);
    cleanup();

    return 1;
}

