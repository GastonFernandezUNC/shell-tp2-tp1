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

int check_redir(char** args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if ((strcmp(args[i], ">") == 0) || (strcmp(args[i], "<") == 0))
        {
            return 0;
        }
        i++;
    }
    return -1;
}

void redir_function(char** args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">") == 0)
        {
            args[i] = NULL;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            return;
        }
        else if (strcmp(args[i], "<") == 0)
        {
            args[i] = NULL;
            int fd = open(args[i + 1], O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
            return;
        }
        i++;
    }
}

int check_pipe(char** args)
{

    int i = 0, pipes = -1;
    while (args[i] != NULL)
    {
        if ((strcmp(args[i], "|") == 0))
        {
            pipes == -1 ? pipes = 0 : pipes;
            pipes++;
        }
        i++;
    }

    int commands = pipes + 1;
    return commands;
}

void pipe_function(char** args, int command_count)
{
    int pipes[command_count - 1][2];
    for (int i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    int cmd_start = 0; // Índice inicial del comando actual
    for (int i = 0; i < command_count; i++)
    {
        if (fork() == 0)
        {
            // Redirigir entrada estándar si no es el primer comando
            if (i != 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            // Redirigir salida estándar si no es el último comando
            if (i != command_count - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Cerrar todos los pipes en el hijo
            for (int j = 0; j < command_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Construir los argumentos del comando actual
            char* cmd_args[100]; // Máximo de 100 argumentos
            int k = 0;
            while (args[cmd_start] != NULL && strcmp(args[cmd_start], "|") != 0)
            {
                cmd_args[k++] = args[cmd_start++];
            }
            cmd_args[k] = NULL; // Terminar lista de argumentos

            // Ejecutar el comando
            execvp(cmd_args[0], cmd_args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // Avanzar cmd_start al siguiente comando (saltando el '|')
        while (args[cmd_start] != NULL && strcmp(args[cmd_start], "|") != 0)
        {
            cmd_start++;
        }
        cmd_start++; // Saltar el '|'
    }

    // Cerrar todos los pipes en el proceso principal
    for (int i = 0; i < command_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Esperar a los procesos hijos
    for (int i = 0; i < command_count; i++)
    {
        wait(NULL);
    }
}

int special_functions(char** args, char* PWD, char* OLDPWD, int* background_processes, int* monitor_pid)
{

    if ((strcmp(args[0], "exit") == 0) || (strcmp(args[0], "quit") == 0))
    {
        if(*monitor_pid != -1){
            stop_monitor(background_processes, monitor_pid);
        }
        return EXIT;
    }

    else if (strcmp(args[0], "cd") == 0)
    {
        handle_cd(args, PWD, OLDPWD);
        return CONTINUE; // Skip forking and executing a program
    }

    else if (strcmp(args[0], "clr") == 0)
    {
        strcpy(args[0], "clear");
        return FORK; // Fork and execute the program
    }

    if(strcmp(args[0], "start_monitor") == 0){
        start_monitor(background_processes, monitor_pid);
        return CONTINUE;
    }

    if(strcmp(args[0], "stop_monitor") == 0){
        stop_monitor(background_processes, monitor_pid);
        return CONTINUE;
    }

    if(strcmp(args[0], "status_monitor") == 0){
        status_monitor(background_processes, monitor_pid);
        return CONTINUE;
    }

    if(strcmp(args[0], "config_monitor") == 0){
        config_monitor(args);
        return CONTINUE;
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