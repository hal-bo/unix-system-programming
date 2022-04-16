// 61911650 高野遥斗

# include "getargs.h"
char *next(char *str)
{
	while (*str && !isspace(*str))
		str++;
	while (*str && isspace(*str))
		str++;
	return str;
}

int	split_size(char *str)
{
	int size;

	size = 0;
	while (*str)
	{
		str = next(str);
		size++;
	}
	return (size);
}

char **do_split(char **ret, char *str, int size)
{
	int i;
	int j;

	i = 0;
	while (i < size)
	{
		j = 0;
		while (str[j] && !isspace(str[j]))
			j++;
		if (!(ret[i] = malloc(sizeof(char) * (j + 1))))
			return 0;
		j = 0;
		while (str[j] && !isspace(str[j]))
		{
			ret[i][j] = str[j];
			j++;
		}
		ret[i][j] = 0;
		str = next(str);
		i++;
	}
	ret[i] = 0;
	return ret;
}

char **getargs(char *str, int *argc, char *argv[])
{
	while (*str && isspace((*str)))
		str++;
    *argc = split_size(str);
	return do_split(argv, str, *argc);
}
