#include "mysh.h"

#define TOKEN_DELIM " \t\r\n\a"

line *split_line(char *input_line)
{
    line *command_line = NULL;
    block *command_block = NULL;
    token command_token;

    command_token = strtok(input_line, TOKEN_DELIM);
    command_token_right = strtok(NULL, TOKEN_DELIM);

    while (command_token != NULL) {
        if (strcmp(command_token, "|") == 0) {
            if (command_block == NULL) {
                fprintf(stderr, "syntax error\n");
                return NULL;
            }
            if (command_token_right == NULL) {
                fprintf(stderr, "syntax error\n");
                return NULL;
            }

            command_line = append_block(command_line, *command_block, REDIRECT_OFF);
            command_block = NULL;

        } else if (strcmp(command_token, ">") == 0) {
            if (command_block == NULL) {
                fprintf(stderr, "syntax error\n");
                return NULL;
            }
            if (command_token_right == NULL) {
                fprintf(stderr, "syntax error\n");
                return NULL;
            }

            command_block = append_token(command_block, command_token_right);
            command_line = append_block(command_line, *command_block, REDIRECT_ON);
            command_token = command_token_right;
            command_token_right = strtok(NULL, TOKEN_DELIM);
            command_block = NULL;

        } else {
            command_block = append_token(command_block, command_token);
        }

        command_token = command_token_right;
        command_token_right = strtok(NULL, TOKEN_DELIM);
    }

    if (command_block != NULL) {
        command_line = append_block(command_line, *command_block, REDIRECT_OFF);
    }

    return command_line;
}

line *append_block(line *line_header, block block_data,  int redirect_flg)
{
    line *top_line = line_header;
    line *next_line = malloc(sizeof(line));
    if (next_line == NULL) {
        fprintf(stderr, "malloc: memory allocation error\n");
    }

    next_line->redirect_flg = redirect_flg;
    next_line->blk = block_data;
    next_line->next_line = NULL;

    if (line_header == NULL) {
        return next_line;
    }

    next_line->next_line = line_header;

    return next_line;
}

block *append_token(block *blk_header, token token_data)
{
    block *top_blk = blk_header;
    block *next_block = malloc(sizeof(block));

    next_block->tok = token_data;
    next_block->next_block = NULL;

    if (blk_header == NULL) {
        return next_block;        
    }

    while(blk_header->next_block != NULL) {
        blk_header = blk_header->next_block;
    }
    blk_header->next_block = next_block;

    return top_blk;
}