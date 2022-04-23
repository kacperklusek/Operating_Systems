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

#define MAX_MSG_LEN 1024
#define MAX_NO_CLIENTS 128
#define MESSAGE_SIZE (MAX_MSG_LEN - 1 - (sizeof(((Message *)0)->type)))


const char *SERVER_KEY_PATH = "./server.c";
const uint PROJECT_ID = 0xAAAA;
const uint PERMISSIONS = 0644;

int create_msg_queue(const char *path, int ftok_flags, int ipc_flags) {
    key_t key;

    if ((key = ftok(path, ftok_flags)) == -1) {
        perror("ftok");
        exit(1);
    }

    int msgid;
    if ((msgid = msgget(key, ipc_flags)) == -1) {
        perror("msgget");
        exit(1);
    }

    if (ipc_flags & IPC_CREAT) {
        printf("Queue created: %d\n", msgid);
    } else {
        printf("Queue opened: %d\n", msgid);
    }

    return msgid;
}

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
    int que_id;
    int index;
    int pid;
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

