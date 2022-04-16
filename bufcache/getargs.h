// 61911650 高野遥斗

#ifndef GETARGS_H
# define GETARGS_H
# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>

char *next(char *str);
int	split_size(char *str);
char **do_split(char **ret, char *str, int size);
char **getargs(char *str, int *argc, char *argv[]);

#endif