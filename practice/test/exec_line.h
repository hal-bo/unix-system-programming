
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "split_line.h"

int execute(line *);
int launch(line *command_line, int i);
int exec_builtin_cmd(char **block_array);
int redirect(line *command_line, int th_of_command, char **block_array);
int pipe_command(line *command_line, int th_of_command, char **block_array);
int is_text(char *filename);

