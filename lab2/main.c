#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "stdlib.h"


// returns 0 if is newline else 1;
int is_newline(char c) {
    if (c != ' ' && c!= '\t' && isspace(c)) {
        return 0;
    } else {
        return 1;
    }
}

void sys_del_ws(char* path_from, char* path_to) {
    int BUFFER_SIZE=1024;

    char block[BUFFER_SIZE];
    char correct_block[BUFFER_SIZE];
    char line_buffer[BUFFER_SIZE];
    int read_size;
    int file_in = open(path_from, O_RDONLY);
    int file_out = open(path_to, O_WRONLY | O_CREAT, S_IRUSR|S_IWUSR);

    char prev = 'a';
    int nws_flag;
    int j;
    int cb_size;
    while ((read_size=read(file_in, block, sizeof (block))) > 0) {
        cb_size = 0;

        // check for empty lines and correct them
        for (int i = 0; i < read_size; ++i) {
            // if prev was \n and current is whitespace
            if (is_newline(prev) == 0 && isspace(block[i]) != 0) {
                nws_flag = 0;
                j = 0;
                for (; is_newline(block[i]) != 0 && i < read_size; i++) {
                    if (isspace(block[i]) == 0 && nws_flag == 0) {nws_flag = 1;}
                    line_buffer[j] = block[i];
                    j++;
                }
                // now if that wasn't a blank line, write it; else set prev
                if (nws_flag == 1) {
                    line_buffer[j++] = '\n';
                    for (int k = 0; k < j; ++k) {
                        correct_block[cb_size] = line_buffer[k];
                        cb_size++;
                        prev = correct_block[cb_size-1];
                    }
                } else {
                    prev = block[i];
                }
            }
            else {
                correct_block[cb_size] = block[i];
                cb_size++;
                prev = correct_block[cb_size-1];
            }
        }

        if (write(file_out, correct_block, cb_size) != cb_size) {
            puts("error writing to file");
            exit(2);
        }
    }
}



int main(int argc, char** argv) {

    if (argc < 3) {
        printf("M8 you have to add at least two parameters, %d were given\n", argc);
        exit(1);
    }

    sys_del_ws(argv[1], argv[2]);


    return 0;
}


