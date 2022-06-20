// 61911650 高野遥斗
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef char * token;

typedef struct block {
    token tok;
    struct block *next_block;
} block;

typedef struct line {
    int redirect_flg;
    block blk;
    struct line *next_line;
} line;

line *split_line(char *);
void print_line(line *);
int num_of_block(line *);
int num_of_token(block *blk);
char **block_to_array(block *);