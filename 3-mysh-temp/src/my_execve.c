#include "header_sh.h"


char    *help_slash_merge(char const *origin, char const *other)
{
	char	*merged;
	char	*tmp;

	if (!origin || !other)
		return (NULL);
	tmp = malloc(strlen(origin) + strlen(other) + 2);
	if (!tmp)
		return (NULL);
	merged = tmp;
	while (*origin)
		*tmp++ = *origin++;
	*tmp++ = '/';
	while (*other)
		*tmp++ = *other++;
	*tmp = '\0';
	return (merged);
}

char	*substr(char const *s, unsigned int start, size_t len)
{
    int 	i;
    char 	*dest;
    mem_alloc(dest, char, len + 1, EXIT_FAILURE);
 
    for (i = start; i < len && (*(s + i) != '\0'); i++)
    {
        *dest = *(s + i);
        dest++;
    }
 
    *dest = '\0';

    return dest - len;
}

char 	*get_absolute_path(char *cmd) 
{
    if (cmd[0] == '/' || cmd[0] == '.')
		return (cmd);

    char *selected_envp;
    selected_envp = getenv("PATH");

    char	*file;
	char	*dir;
	int		diff;

	while (*selected_envp)
	{
		diff = strchr(selected_envp, ':') - selected_envp;
		if (diff < 0)
			diff = strlen(selected_envp);
		dir = substr(selected_envp, 0, diff);

		file = help_slash_merge(dir, cmd);

		free(dir);
		if (access(file, X_OK) == 0)
			return (file);
		free(file);
		if (strlen(selected_envp) < (size_t)diff)
			break ;
		selected_envp += diff;
		if (*selected_envp)
			selected_envp++;
	}
    fprintf(stderr, "get_absolute_path : command not found\n");
	exit(EXIT_FAILURE);
	return (NULL);

}

