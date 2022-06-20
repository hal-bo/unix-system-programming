// 61911650 高野遥斗

#ifndef BUILTIN_CMD_H
# define BUILTIN_CMD_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void exit_proc(int, char *[]);
void cd_proc(int, char *[]);
int exec_builtin_cmd(int argc, char *argv[]);

#endif