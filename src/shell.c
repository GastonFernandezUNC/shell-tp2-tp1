#include "shell.h"

// Function to read and parse the command
void read_command(char* cmd, char* CWD, char* USER, char* HOSTNAME)
{
    getCurrentPath(CWD, USER, HOSTNAME);
    fgets(cmd, MAX_CMD_LEN, stdin);
    cmd[strcspn(cmd, "\n")] = 0; // Remove the newline at the end of the input
}

// Function to parse the command into program and arguments
int parse_command(char* cmd, char** args)
{
    int i = 0;
    args[i] = strtok(cmd, " "); // Get the first argument

    while (args[i] != NULL)
    {
        i++;
        args[i] = strtok(NULL, " "); // Get the next arguments
    }

    env_vars(args, i);

    return i; // Return the number of arguments
}

void create_fork(char** args, bool background, int* current_child, int* background_processes)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // In the child process: Execute the command
        if (check_redir(args) == 0)
        {
            redir_function(args);
        }

        if (execvp(args[0], args) == -1)
        {
            perror("execvp failed");
            exit(1);
        }
    }
    else if (pid > 0)
    {
        // In the parent process: Wait for the child to finish
        if (!background)
        {
            int status;
            // wait(NULL);
            *current_child = pid;
            waitpid(pid, &status, WUNTRACED);

            if (WIFSTOPPED(status))
            {
                printf("\n%d Stopped\n", pid);
            }
            else if (WIFSIGNALED(status))
            {
                printf("\n%d Killed\n", pid);
            }
        }
        else
        {
            printf("[%d] %d\n", ++(*background_processes), pid);
            background = false;
        }
        *current_child = -1;
    }
    else
    {
        perror("fork failed");
        exit(1);
    }
}

void sig_handler(int signo, int* current_child)
{
    if (*current_child > 0)
    {
        // Envía SIGINT al proceso hijo actual
        kill(*current_child, signo);
        // printf("");
        *current_child = -1;
    }
    else
    {
        printf("\n");
    }
}
