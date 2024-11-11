#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 1024
#define MAX_CWD_BUFFER 1024

#define MAX_ARGS 100


char cwd[MAX_CWD_BUFFER];

void curr_path(){
    if(getcwd(cwd,sizeof(cwd)) == NULL){
        perror("PATH LENGTH OUT OF BOUND");
    }

    printf("%s$ ", cwd);
}

// Function to read and parse the command
void read_command(char *cmd) {
    curr_path();
    fgets(cmd, MAX_CMD_LEN, stdin);
    cmd[strcspn(cmd, "\n")] = 0;  // Remove the newline at the end of the input
}

// Function to parse the command into program and arguments
int parse_command(char *cmd, char **args) {
    int i = 0;
    args[i] = strtok(cmd, " ");  // Get the first argument

    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");  // Get the next arguments
    }
    return i;
}

void handle_cd(char **args) {

    if (args[1] == NULL) {
        // If no argument is passed, change to the home directory
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME not set\n");

        } else if (chdir(home) != 0) {
            perror("cd");
        }

    } else {
        // If a directory is passed, try to change to it
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

// Main function
int main() {
    char cmd[MAX_CMD_LEN];  // Command buffer
    char *args[MAX_ARGS];   // Array of arguments

    while (1) {
        read_command(cmd);  // Get the command from user
        if (strlen(cmd) == 0) continue;  // Skip empty input

        // Parse the command into program and arguments
        int arg_count = parse_command(cmd, args);

        // Check for exit condition
        if (strcmp(args[0], "exit") == 0 ){
            break;
        }

        // Check for change directory
        if (strcmp(args[0], "cd") == 0) {
            handle_cd(args);
            continue;  // Skip forking and executing a program
        }

        // Create a child process to execute the command
        pid_t pid = fork();
        if (pid == 0) {
            // In tyhe child process: Execute the command
            if (execvp(args[0], args) == -1) {
                perror("execvp failed");
                exit(1);
            }
        } else if (pid > 0) {
            // In the parent process: Wait for the child to finish
            wait(NULL);
        } else {
            perror("fork failed");
            exit(1);
        }
    }

    return 0;
}
