#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_CMD_LEN 512
#define MAX_OUTPUT_LEN 512
#define MAX_ARGS_NUM 16
#define WRITE_PIPE 1
#define READ_PIPE 0


char ** parse_args(char* command) {
    char delims[3] = {' ', '\n', '\t'};
    int arg_num = 0;
    char **args = NULL;
    char *arg = strtok(command, delims);
    while(arg != NULL){
        arg_num++;
        args = realloc(args, sizeof (char*) * arg_num);
        args[arg_num-1] = strdup(arg);
        arg = strtok(NULL, delims);
    }
    args = realloc(args,sizeof (char*) * (arg_num + 1));
    args[arg_num] = NULL;

    return args;
}


void handle_commands(char** cmds, int n_of_cmds) {
    int pipes[2][2];

    int i;
    for (i = 0; i < n_of_cmds; ++i) {
        if (i > 0) {
            close(pipes[i % 2][READ_PIPE]);
            close(pipes[i % 2][WRITE_PIPE]);
        }
        if (pipe(pipes[i%2]) == -1) {
            puts("error piping");
            exit(EXIT_FAILURE);
        }

        pid_t cp = fork();
        if (cp == 0) { // gówniarz
            char **args = parse_args(cmds[i]);

            if (i != n_of_cmds - 1) {
                close(pipes[i % 2][READ_PIPE]);
                dup2(pipes[i%2][WRITE_PIPE], STDOUT_FILENO);
            }
            if (i != 0) {
                close(pipes[(i + 1) % 2][WRITE_PIPE]);
                dup2(pipes[(i + 1) %2][READ_PIPE], STDIN_FILENO);
            }
//            printf("executing %s\n", args[0]);
            execvp(args[0], args);

            exit(EXIT_SUCCESS);
        }
    }
    close(pipes[i % 2][READ_PIPE]);
    close(pipes[i % 2][WRITE_PIPE]);
    wait(0);
    exit(0);
}

char *trim_end(char* s) {
    char *end = s + strlen(s) - 1;
    while (isspace(*end)){
        *end = '\0';
        end--;
    }
    return s;
}

void handle_line(char* cmd_in) {
    char cmdln[MAX_CMD_LEN];
    strcpy(cmdln, cmd_in);
    printf("SIEMAAAAAAAAAAAAAAAAAAAAAAAA\n %s\n", cmdln);
    int n_args = 1;
    char *c = trim_end(cmdln);
    while (*c != '\0') {
        if (*c == '|'){
            n_args++;
        }
        c++;
    }

    char ** cmds = malloc(n_args * sizeof (char*));
    for (int i = 0; i < n_args; ++i) {
        cmds[i] = malloc(MAX_CMD_LEN * sizeof (char*));
    }

    char *cmd = strtok(cmdln, "|");
    int index = 0;
    while(cmd != NULL){
        strcpy(cmds[index++], cmd);
        cmd = strtok(NULL, "|");
    }

    pid_t pid = fork();     // może tego forka do ifa wrzucic ?
    if (pid == 0) { // gówniarz
        handle_commands(cmds, n_args);
        exit(0);
    }
    waitpid(pid, NULL, 0);

    for (int i = 0; i < n_args; ++i) {
        free(cmds[i]);
    }
    free(cmds);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        puts("give me at least one argument bro");
        return 1;
    }
    const char* filename = argv[1];

    FILE* commands_file = fopen(filename, "r");
    if (!commands_file) {
        puts("error reading filename");
        return 1;
    }

    char buffer[MAX_CMD_LEN];
    while(fgets(&buffer[0], sizeof(buffer), commands_file) != NULL){
        handle_line(&buffer[0]);
    }

    fclose(commands_file);

    return 0;
}
