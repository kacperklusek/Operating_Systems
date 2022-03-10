#include <stdlib.h>
#include "stdio.h"

#include "library.c"

int main() {
    int TABLE_SIZE = 10;

    blocks_table table =  create_table(TABLE_SIZE);

    char* path1 = "../test1.txt";
    char* path2 = "../test2.txt";
    char** paths = calloc(2, sizeof (char*));
    paths[0] = path1;
    paths[1] = path2;

    FILE* file = wc_files(paths);

    int index = save_block(file, &table);

    printf("it works here: %d %d %d %s \n", table.blocks[index].blockContent[0].lines,
           table.blocks[index].blockContent[0].words,
           table.blocks[index].blockContent[0].characters,
           table.blocks[index].blockContent[0].path);

    remove_block(&table, index);
    printf("removed block \n");

    printf("and it does not work here no mo': %d %d %d %s \n", table.blocks[index].blockContent[0].lines,
           table.blocks[index].blockContent[0].words,
           table.blocks[index].blockContent[0].characters,
           table.blocks[index].blockContent[0].path);

    fclose(file);
    puts("closed file");

    return 0;
}