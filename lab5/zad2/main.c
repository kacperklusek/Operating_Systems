#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFF_SIZE 512
#define MAIL_CMD_SIZE 128
#define WRITE_PIPE 1
#define READ_PIPE 0
#define EMAIL_COLUMN 2
#define DATE_COLUMN 3


int main(int argc, char* argv[]) {
    if (argc == 2)
    {
        char* key = argv[1];
        char command[10] = "sort -k ";
        if (strcmp(key, "nadawca") == 0) { strcat(command, "2"); }
        if (strcmp(key, "data") == 0) { strcat(command, "3"); }
        FILE *sort_input = popen(command, "w");
        FILE *mail_output = popen("mailq", "r");
        char buff[BUFF_SIZE];
        while(fgets(buff, sizeof(buff), mail_output) != NULL){
            fputs(buff, sort_input);
        }
        pclose(mail_output);
        pclose(sort_input);

    }
    else if (argc == 4)
    {
        char* email_address = argv[1];
        char* title = argv[2];
        char* message = argv[3];

        char commmand[MAIL_CMD_SIZE] = "mail -s ";
        strcat(strcat(strcat(commmand, "'"), title), "' ");
        strcat(strcat(strcat(commmand, "'"), email_address), "' ");

        FILE *mail_input = popen(commmand, "w");
        fputc('"', mail_input);
        fputs(message, mail_input);
        fputc('"', mail_input);
        pclose(mail_input);
    }
    return 0;
}
