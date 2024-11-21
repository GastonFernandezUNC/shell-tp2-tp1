#include "handlers.h"

// Function to handle environment variables
void env_vars(char** args, int args_count)
{
    for (int i = 0; i < args_count; i++)
    {
        if (strstr(args[i], "$") != NULL)
        {
            char* env_var = getenv(args[i] + 1);
            if (env_var != NULL)
            {
                args[i] = env_var;
            }
        }
    }
}

int special_functions(char** args, char* PWD, char* OLDPWD)
{

    if ((strcmp(args[0], "exit") == 0) || (strcmp(args[0], "quit") == 0))
    {
        return -1;
    }

    if (strcmp(args[0], "cd") == 0)
    {
        handle_cd(args, PWD, OLDPWD);
        return 0; // Skip forking and executing a program
    }

    if (strcmp(args[0], "clr") == 0)
    {
        strcpy(args[0], "clear");
        return 1;
    }

    return -2;
}

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
        getcwd(tmp, MAX_CWD_BUFFER);

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
            getcwd(OLDPWD, MAX_CWD_BUFFER);
            // Change to the specified directory
            if (chdir(args[1]) != 0)
            {
                perror("cd");
            }
            // Save the new directory as the current directory
            getcwd(PWD, MAX_CWD_BUFFER);
        }
    }
}

char** commands_file(char* file, int* lines_amount)
{
    FILE* fp;

    // Opening file
    fp = fopen(file, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return NULL;
    }
    else
    {
        char** lines = NULL;
        char buffer[FILE_BUFFER_SIZE];
        *lines_amount = 0;

        while (fgets(buffer, sizeof(buffer), fp))
        {
            // Eliminar salto de línea
            buffer[strcspn(buffer, "\n")] = '\0';

            // Aumentar el tamaño del arreglo dinámico
            lines = realloc(lines, (*lines_amount + 1) * sizeof(char*));
            if (!lines)
            {
                perror("Error reallocating memory");
                fclose(fp);
                return NULL;
            }

            // Almacenar la línea en el arreglo
            lines[*lines_amount] = strdup(buffer);
            if (!lines[*lines_amount])
            {
                perror("Error duplicating string");
                fclose(fp);
                return NULL;
            }

            (*lines_amount)++;
        }

        fclose(fp);
        return lines;
    }
}