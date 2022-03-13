#include "lib.h"

typedef struct node {
  struct node* next;
  char* value;
} node;

// node** tail, char* value
#define push_back(tail, val) \
  *tail = calloc(sizeof(struct node), 1); \
  (*tail)->value = val; \
  tail = &(*tail)->next;

static block* create_block(int size) {
  block* b = calloc(sizeof(block), 1);
  b->size = size;
  b->operations = calloc(sizeof(char*), size);
  return b;
}

block* compare_files(char* file1, char* file2) {
  // initializing variables for diff call
  static const char pattern[] = "diff %s %s > /tmp/diffs";

  int size = sizeof(pattern) + strlen(file1) + strlen(file2);
  use(char, out, size, {
    sprintf(out, pattern, file1, file2);
    system(out);
  });

  // reading file
  FILE *f = fopen("/tmp/diffs", "r");

  if (f == NULL) return NULL;

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  node* operation_list = NULL;
  node** tail = &operation_list;
  int operation_count = 0;

  use_malloc(char, content, fsize + 1, {
    fread(content, 1, fsize, f);
    content[fsize] = 0;
    fclose(f);

    for (int i = 0, operation_start, operation_end; content[i];) {
      operation_start = i;
      do {
        while (content[i] != '\n' && content[i]) i++;
        if (content[i]) i++;
      } while (content[i] && (content[i] == '<' || content[i] == '-' || content[i] == '>' || content[i] == '\\'));
      operation_end = i;

      int operation_len = operation_end - operation_start + 1;
      char* operation = calloc(sizeof(char), operation_len + 1);

      for (int i = operation_start, j = 0; i < operation_end; ++i, ++j)
        operation[j] = content[i];
      
      push_back(tail, operation);
      operation_count++;
    } 

  });

  block* b = create_block(operation_count);
  b->files[0] = file1;
  b->files[1] = file2;

  struct node* iter = operation_list, *tmp;
  for (int i = 0; i < operation_count; i++) {
    b->operations[i] = iter->value;
    tmp = iter;
    iter = iter->next;
    free(tmp);
  }

  return b;
}

table* create_table(int size) {
  table* t = calloc(sizeof(table), 1);
  t->values = calloc(sizeof(block*), size);
  t->size = size;
  return t;
}

void remove_block(table* table, int block_index) {
  if (table->values[block_index] == NULL) return;
  free(table->values[block_index]);
  table->values[block_index] = NULL;
}

void remove_operation(table* table, int block_index, int operation_index) {
  block* block = table->values[block_index];
  if (block == NULL) return;
  if (block->operations[operation_index] == NULL) return;
  free(block->operations[operation_index]);
  block->operations[operation_index] = 0;
}

void compare_file_sequence(table* table, char** sequence) {
  static char* files[2];

  int size = table->size;
  for (int i = 0; i < size; i++) {
    if (table->values[i]) free(table->values[i]);
    files[0] = strtok(sequence[i], ":");
    files[1] = strtok(NULL, "");
    table->values[i] = compare_files(files[0], files[1]);
  }
}

int operation_count(table* table, int block_index) {
  if (table == NULL || table->values == NULL) return -1;
  block* block = table->values[block_index];
  int count = 0;
  for (int i = 0; i < block->size; i++)
    if (block->operations[i]) count++;
  return count;
}

int blocks_count(table* table) {
  if (table == NULL || table->values == NULL) return -1;
  int count = 0;
  for (int i = 0; i < table->size; i++) 
    if (table->values[i]) count++;
  return count;
}
