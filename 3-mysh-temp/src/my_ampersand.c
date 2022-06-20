#include "header_sh.h"

int     check_ampersand(char ***cmds)
{
    int i, j;

    for (i = 0; cmds[i] != NULL; i++)
        ;

    for (j = 1; cmds[i - 1][j] != NULL; j++)
        ;

    if (cmds[i - 1][j - 1][0] == '&')
    {
        free(cmds[i - 1][j - 1]);
        cmds[i - 1][j - 1] = NULL;
        return 1;
    }
    else
        return 0;
    
    return 0;
}