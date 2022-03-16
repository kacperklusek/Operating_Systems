#include "library.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

blocks_table create_table(int no_blocks) {
    blocks_table table = {
            calloc(no_blocks, sizeof(block)),
            no_blocks,
            0
    };

    return table;
}

FILE* wc_files(char* paths) {
    FILE *outputfile, *fp;

    outputfile = tmpfile();

    int size = 13 + strlen(paths);
    char* first = "wc ";
    char command[size];
    strcpy(command, first);
    strcat(command, paths);

    int var_size = strlen(paths) * 3 + 10;
    char var[var_size];
    char prev[var_size];

    fp = popen(command, "r");

    while (fgets(var, var_size, fp) != NULL){
        strcpy(prev, var);
        fprintf(outputfile, "%s", var);
    }
    pclose(fp);
    rewind(outputfile);

    return outputfile;
}


int get_free_index(blocks_table table) {
    return table.amount;
}

int get_no_lines(FILE* file) {
    int lines_count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') lines_count++;
    }
    rewind(file);
    return lines_count;
}

int save_block(FILE* file, blocks_table* table) {
    int i = 0;
    int free_index = get_free_index(*table);
    int number_of_files = get_no_lines(file);
    long lines, words, characters;
    table->blocks[free_index].blockContent = calloc(number_of_files, sizeof (block_content));
    table->blocks[free_index].size = number_of_files;

    char* path = calloc(60, sizeof (char));
    while (fscanf(file, "%ld %ld %ld %s \n", &lines, &words, &characters, path) != EOF) {
//        printf("READING:   %ld %ld %ld %s \n", lines, words, characters, path);
        block_content content;
        content.lines = lines;
        content.words = words;
        content.characters = characters;
        content.path = calloc(60, sizeof (char));
        strcpy(content.path, path);

        table->blocks[free_index].blockContent[i] = content;
        path = calloc(60, sizeof (char));
        i++;
    }
    fclose(file);
    table->amount++;

    return free_index;
}

void free_content(block_content* content) {
    free(content->path);
}

void free_block(block* actual_block){
    for (int i=0; i < actual_block->size; i++){
        free_content(&actual_block->blockContent[i]);
    }
    free(actual_block->blockContent);
}

void remove_block(blocks_table* table, int index) {
    if (index < 0 || index >= table->amount) {
        printf("wrong index! not 0 <= %d < %d\n", index, table->amount); return;
    }
    // free block
    free_block(&table->blocks[index]);
    // shift next blocks by one
    for (int i = index + 1; i < table->amount; i++) {
        table->blocks[i-1] = table->blocks[i];
    }

    table->amount--;
}


