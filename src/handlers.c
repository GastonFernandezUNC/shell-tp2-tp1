#include "handlers.h"

void getCurrentPath(char* CWD, char* USER, char* HOSTNAME)
{
    char tmp[MAX_CWD_BUFFER];
    if (getcwd(tmp, sizeof(tmp)) == NULL)
    {
        perror("PATH LENGTH OUT OF BOUND");
    }
    else
    {
        strcpy(CWD, tmp);
    }
    printf("%s@%s:%s$  ", USER, HOSTNAME, CWD);
}

void handle_cd(char** args, char* PWD, char* OLDPWD)
{

    if (args[1] == NULL || strcmp(args[1], "~") == 0)
    {
        char tmp[MAX_CWD_BUFFER];
        getcwd(tmp, sizeof(tmp));

        // If no argument is passed, change to the home directory
        char* home = getenv("HOME");
        if (home == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
        }
        else if (chdir(home) != 0)
        {
            perror("cd");
        }
        else
        {

            strcpy(OLDPWD, tmp);
            strcpy(PWD, home);
        }
    }

    else
    {
        // If a directory is passed, try to change to it
        if (strcmp(args[1], "-") == 0)
        {
            // If the argument is "-", change to the previous directory

            // Swap the current directory with the old directory
            char tmp[MAX_CWD_BUFFER];
            strcpy(tmp, PWD);
            strcpy(PWD, OLDPWD);
            strcpy(OLDPWD, tmp);

            if (chdir(PWD) != 0)
            {
                perror("cd");
            }
        }
        else
        {
            // Save the current directory as the old directory
            getcwd(OLDPWD, sizeof(OLDPWD));
            // Change to the specified directory
            if (chdir(args[1]) != 0)
            {
                perror("cd");
            }
            // Save the new directory as the current directory
            getcwd(PWD, sizeof(OLDPWD));
        }
    }
}
