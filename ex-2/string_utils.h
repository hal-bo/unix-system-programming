// 61911650 高野遥斗

#ifndef GETARGS_H
# define GETARGS_H
# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>

int is_str_numeric(char *str);
char *next_word(char *str);
int	get_word_num(char *str);
char **do_split(char **ret, char *str, int size);
char **getargs(char *str, int *argc, char *argv[]);

#endif