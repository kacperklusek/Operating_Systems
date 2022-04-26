#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>

#include "common.h"

static mqd_t client_queue_id;
static mqd_t server_queue_id;
static int client_index;
char fname[128];


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
            // ???
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

void register_event(){
    struct sigevent e;

    e.sigev_notify_function = process_message;
    e.sigev_notify = SIGEV_THREAD;
    e.sigev_value.sival_ptr = NULL;
    e.sigev_notify_attributes = NULL;

    mq_notify(client_queue_id, &e);
}

int main(int argc, char** argv) {
    signal(SIGINT, cleanup);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_LEN;
    attr.mq_curmsgs = 0;

    sprintf(&fname[0], "/client_%d", getppid());
    client_queue_id = mq_open(&fname[0], O_RDWR | O_CREAT, PERMISSIONS, &attr);
    printf("Opened queue %s %d\n", fname, client_queue_id);

    client_index = init_connection_to_server();
    printf("My ID: %d\n", client_index);

    register_event()
    listen();

    return 1;
}

