#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
// #include "handlers.h"

#define MAX_CMD_LEN 1024
#define MAX_CWD_BUFFER 1024

#define MAX_ARGS 100

char CWD[MAX_CWD_BUFFER], PWD[MAX_CWD_BUFFER], OLDPWD[MAX_CWD_BUFFER];
char *USER, *HOSTNAME;

void getCurrentPath()
{

    if (getcwd(CWD, sizeof(CWD)) == NULL)
    {
        perror("PATH LENGTH OUT OF BOUND");
    }

    printf("%s@%s:%s$  ", USER, HOSTNAME, CWD);
}

// Function to read and parse the command
void read_command(char* cmd)
{
    getCurrentPath();
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
    return i; // Return the number of arguments
}

void handle_cd(char** args)
{

    if (args[1] == NULL || strcmp(args[1], "~") == 0)
    {
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
        strcpy(OLDPWD, home);
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

// Main function
int main()
{
    char cmd[MAX_CMD_LEN]; // Command buffer
    char* args[MAX_ARGS];  // Array of arguments
    USER = getenv("USER");
    HOSTNAME = getenv("HOSTNAME");

    while (1)
    {
        read_command(cmd); // Get the command from user
        if (strlen(cmd) == 0)
            continue; // Skip empty input

        // Parse the command into program and arguments
        int arg_count = parse_command(cmd, args);

        // Check for exit condition
        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }

        // Check for change directory
        if (strcmp(args[0], "cd") == 0)
        {
            handle_cd(args);
            continue; // Skip forking and executing a program
        }

        if (strcmp(args[0], "clr") == 0)
        {
            strcpy(args[0], "clear");
        }

        bool background = false;
        if (strcmp(args[arg_count - 1], "&") == 0)
        {
            args[arg_count - 1] = NULL;
            background = true;
        }

        // Create a child process to execute the command
        pid_t pid = fork();
        if (pid == 0)
        {
            // In the child process: Execute the command
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
                // wait(NULL);
                waitpid(pid, NULL, 0);
            }
            else
            {
                printf("Process running in background with PID %d\n", pid);
                background = false;
            }
        }
        else
        {
            perror("fork failed");
            exit(1);
        }
    }

    return 0;
}
