#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H

#include <bits/types/FILE.h>

typedef struct
{
    int lines;
    int words;
    int characters;
    char* path;
} block_content;

typedef struct
{
    int size;
    block_content * blockContent;
} block;

typedef struct
{
    block* blocks;
    int size;
    int amount;
} blocks_table;


blocks_table create_table(int no_blocks);
FILE* wc_files(char* paths[]);
int save_block(FILE* file, blocks_table* table);
void remove_block(blocks_table* table, int index);


#endif //LAB1_LIBRARY_H
