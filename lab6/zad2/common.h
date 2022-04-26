#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>

#define MAX_MSG_LEN 1024
#define MAX_NO_CLIENTS 128
#define MESSAGE_SIZE (MAX_MSG_LEN - 1 - (sizeof(((Message *)0)->type)))


const char *SERVER_KEY_PATH = "./server.c";
const uint PROJECT_ID = 0xAAAA;
const uint PERMISSIONS = 0644;

typedef struct {
    long type;
    int index;
    int to_index;
    char text[MAX_MSG_LEN];
} Message;

typedef enum{
    NOT_CONNECTED = 0,
    CONNECTED,
    LAST_STATUS
} ClientStatus;

typedef struct {
    mqd_t que_id;
    int index;
    int pid;
    char fname[128];
    ClientStatus status;
} ClientData;

typedef enum {
    INIT = 1,
    LIST,
    TOALL,
    TOONE,
    STOP,
    LAST_TYPE
} TaskType;

const char *TaskTypeStr[LAST_TYPE] = {
        "",
        "INIT",
        "LIST",
        "TOALL",
        "TOONE",
        "STOP"
};

int get_message(mqd_t source, Message *m) {
    char message[MAX_MSG_LEN];
    char num[5];
    int ret = mq_receive(source, &message[0], MAX_MSG_LEN, NULL);
    if (ret == -1) {
        return 0;
    }

    m->type = message[0];
    for (int i = 0; i < 4; i++) {
        num[i] = message[i+1];
    }
    num[4] = '\0';
    sscanf(&num[0], "%d", &m->index);
    sprintf(&m->text[0], "%s", message + 5);
    return ret;
}

int send_message_to(mqd_t destination, Message *m){
    uint length = strlen(&m->text[0]) + sizeof (m->type) + sizeof (m->index) + 1;
    char buffer[length];
    buffer[0] = m->type;
    sprintf(buffer + 1, "%04d", m->index);
    sprintf(buffer + 5, "%s", &m->text[0]);
    int ret = mq_send(destination, &buffer[0], length, 0);
    if (ret == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }
    return ret;
}

int send_message(mqd_t destination, char type, int index, char *msg) {
    Message m = {.type=type, .index=index};
    if (msg == NULL) {
        m.text[0] = '\0';
    } else {
        strncpy(&m.text[0], msg, MESSAGE_SIZE);
    }
    return send_message_to(destination, &m);
}
