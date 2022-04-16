// 61911650 高野遥斗

# include "string_utils.h"

int is_str_numeric(char *str)
{
	while (*str) {
		if (!('0' <= *str && *str <= '9')) {
			return 0;
		}
		str++;
	}
	return 1;
}

char *next_word(char *str)
{
	while (*str && !isspace(*str)) {
		str++;
	}
	while (*str && isspace(*str)) {
		str++;
	}
	return str;
}

int	get_word_num(char *str)
{
	int word_num;

	word_num = 0;
	while (*str) {
		str = next_word(str);
		word_num++;
	}
	return word_num;
}

char **do_split(char **ret, char *str, int size)
{
	int i;
	int j;

	i = 0;
	while (i < size) {
		j = 0;
		while (str[j] && !isspace(str[j])) {
			j++;
		}
		if (!(ret[i] = malloc(sizeof(char) * (j + 1)))) {
			return 0;
		}
		j = 0;
		while (str[j] && !isspace(str[j])) {
			ret[i][j] = str[j];
			j++;
		}
		ret[i][j] = 0;
		str = next_word(str);
		i++;
	}
	ret[i] = 0;
	return ret;
}

char **getargs(char *str, int *argc, char *argv[])
{
	while (*str && isspace((*str))) {
		str++;
	}
    *argc = get_word_num(str);
	return do_split(argv, str, *argc);
}
