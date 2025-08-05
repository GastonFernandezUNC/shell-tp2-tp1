#include "handlers.h"
#include "shell.h"
#include <fenv.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
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

// check for the use of the '<' and '>' operators
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

// actually handle the use of the '<' and '>' operators
void redir_function(char** args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">") == 0)
        {
            args[i] = NULL;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                perror("open");
                return;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            return;
        }
        else if (strcmp(args[i], "<") == 0)
        {
            args[i] = NULL;
            int fd = open(args[i + 1], O_RDONLY);
            if (fd == -1)
            {
                perror("open");
                return;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            return;
        }
        i++;
    }
}

// check for the use of the '|' operator
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

// handle the use of the '|' operator
void pipe_function(char** args, int command_count)
{
    int args_iter_0 = 0;
    ;
    char* copy_args[MAX_CMD_LEN];
    // TODO this works by miracle. Use a deep copy instead.
    while (args[args_iter_0] != NULL)
    {
        copy_args[args_iter_0] = args[args_iter_0];
        args_iter_0++;
    }
    copy_args[args_iter_0] = NULL;

    if (copy_args[args_iter_0] == NULL)
    {
        printf("\n");
    }

    if (command_count <= 1)
    {
        perror("Command count\n");
        return;
    }
    int pipes[command_count - 1][2];
    memset(pipes, 0, sizeof(pipes));

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
            char* cmd_args[MAX_ARGS]; // Máximo de 100 argumentos
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

// Function to handle internal and other functions of the shell
int special_functions(char** args, char* PWD, char* OLDPWD, int* background_processes, int* monitor_pid)
{

    if ((strcmp(args[0], "exit") == 0)  || (strcmp(args[0], "quit") == 0 || (strcmp(args[0], "q") == 0)))
    {
        if (*monitor_pid != -1)
        {
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

    if (strcmp(args[0], "start_monitor") == 0)
    {
        start_monitor(background_processes, monitor_pid);
        return CONTINUE;
    }

    if (strcmp(args[0], "stop_monitor") == 0)
    {
        stop_monitor(background_processes, monitor_pid);
        return CONTINUE;
    }

    if (strcmp(args[0], "status_monitor") == 0)
    {
        status_monitor(background_processes, monitor_pid);
        return CONTINUE;
    }

    if (strcmp(args[0], "config_monitor") == 0)
    {
        config_monitor(args);
        return CONTINUE;
    }

    if (strcmp(args[0], "list_config_files") == 0)
    {

        bool f_recursive = false;
        bool f_cat_files = false;

        /* store the path where either 'find' or 'ls' will be applied */
        char path[MAX_CWD_BUFFER] = " ";
        /* pre-set commands to give to the parser */
        char search_command_no_recursive[] = "ls ";
        char search_command_recursive[] = "find ";
        //char grep_command[] = " | grep -P (\\.config|\\.json|\\.yaml)";
        //char cat_command[] = " | xargs -I {} cat {}";

        /* storage variable */
        char** paths = malloc(MAX_READ_STDOUT * sizeof(char*));
        if(!paths)
        {
            perror("Malloc error");
            exit(EXIT_FAILURE);

        }

        /* Iterate through all the arguments, checking for flags and a path */
        int args_iter = 1;
        while (args[args_iter])
        {
            if (!args[args_iter])
            {
                break;
            }

            if (strcmp(args[args_iter], "-r") == 0)
            {
                f_recursive = true;
            }

            else if (strcmp(args[args_iter], "-c") == 0)
            {
                f_cat_files = true;
            }

            /* if not a flag, assume its a path. */
            else
            {
                strcpy(path, args[args_iter]);
            }

            args_iter++;
        }

        /* Nullify previous arguments */
        int args_i = 0;
        while (args[args_i])
        {
            args[args_i] = NULL;
            args_i++;
        }

        int fd[2];
        if(pipe(fd) == -1)
        {
            perror("Pipe creation failed.\n");
            exit(EXIT_FAILURE);
        }

        pid_t fid = fork();
        if(fid == -1)
        {
            close(fd[0]);
            close(fd[1]);
            perror("Fork creation failed.\n");
            return EXIT_FAILURE;
        }

        if(fid == 0)
        {

            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
        
            char search_command[MAX_CMD_LEN] = "";
    
            strcat(search_command, f_recursive ? search_command_recursive : search_command_no_recursive);
            strcat(search_command, path);
            //perror(search_command);
            parse_command(search_command, args);
            execvp(args[0], args);

        }

        else
        {
            close(fd[1]);
            int counter = 0;
			long unsigned int buffer_iter = 0;
            char buffer[MAX_READ_STDOUT];
            while((counter = read(fd[0], buffer, sizeof(buffer)-1)) > 0)
            {
                buffer[counter] = '\0';
			    char* token = strtok(buffer, "\n");
				while (token && buffer_iter < MAX_READ_STDOUT)
				{
				    paths[buffer_iter] = strdup(token); // Allocate memory
				    if (!paths[buffer_iter]) {
				        perror("strdup");
				        break;
				    }
				    buffer_iter++;
				    token = strtok(NULL, "\n");
				}
            }
            close(fd[0]);
            wait(NULL);

			paths[buffer_iter] = NULL; // Null-terminate the list

	        printf("Exploring Directory %s\n", strcmp(path, " ") != 0 ? path:PWD);
	        parse_config_files(paths, f_cat_files);
	
	        for(long unsigned int paths_iter = 0; paths_iter < buffer_iter; paths_iter++)
	        {
	            free(paths[paths_iter]);
	        }
	        free(paths);
	        return CONTINUE;
        }
    }

    return NOTHING;
}

void parse_config_files(char** paths, bool f_cat_files)
{
    int iter = 0;
    while(paths[iter])
    {
        if(strstr(paths[iter],".config") || strstr(paths[iter],".json") || strstr(paths[iter],".yaml"))
        {
            printf("Config File Found: %s\n",paths[iter]);
            if(f_cat_files)
            {
                printf("Contents From %s\n", paths[iter]);
                char* args[] = {"cat", paths[iter], NULL};
                pid_t pid = fork();
                if(pid == -1)
                {
                    perror("Fork failed\n");
                    exit(EXIT_FAILURE);
                }

                if(pid == 0)
                {
                    execvp(args[0],args);
                    perror("execvp failed\n");
                }
                else
                {
                    wait(NULL);
                }
            }
        }

        iter++;
    }

}

// get the current dir
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

// handle de use of the cd command
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

// Function to read the command from a file
int is_empty_or_whitespace(const char* str)
{
    while (*str)
    {
        if (!isspace((unsigned char)*str))
        {
            return 0; // Found a non-whitespace character
        }
        str++;
    }
    return 1; // String is empty or all whitespace
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
            // Remove newline character
            buffer[strcspn(buffer, "\n")] = '\0';

            // Check if the line is empty or contains only whitespace
            if (is_empty_or_whitespace(buffer))
            {
                continue;
            }

            // Increase the size of the dynamic array
            lines = realloc(lines, (*lines_amount + 1) * sizeof(char*));
            if (!lines)
            {
                perror("Error reallocating memory");
                fclose(fp);
                return NULL;
            }

            // Store the line in the array
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
