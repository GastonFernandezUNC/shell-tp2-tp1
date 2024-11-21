#include "handlers.h"

char CWD[MAX_CWD_BUFFER], PWD[MAX_CWD_BUFFER], OLDPWD[MAX_CWD_BUFFER];
char *USER, HOSTNAME[MAX_CMD_LEN];
int background_processes = 0;
int current_child = -1;
bool stop_requested = false;

// Function to read and parse the command
void read_command(char* cmd)
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

void create_fork(int arg_count, char** args, bool background)
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
            current_child = pid;
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
            printf("[%d] %d\n", ++background_processes, pid);
            background = false;
        }
        current_child = -1;
    }
    else
    {
        perror("fork failed");
        exit(1);
    }
}

void sig_handler(int signo)
{
    if (current_child > 0)
    {
        // Envía SIGINT al proceso hijo actual
        kill(current_child, signo);
        // printf("");
        current_child = -1;
    }
    else
    {
        printf("\n");
    }
}

// Main function
int main(int argc, char* argv[])
{

    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGCHLD, sig_handler);


    char cmd[MAX_CMD_LEN]; // Command buffer
    char* args[MAX_ARGS];  // Array of arguments
    USER = getenv("USER");
    gethostname(HOSTNAME, MAX_CMD_LEN);

    if (argc > 1)
    {
        int lines_amount = 0;
        char* file = argv[1];
        char buffer[FILE_BUFFER_SIZE];
        char** commands = commands_file(file, &lines_amount);
        int i = 0;
        while (i < lines_amount)
        {
            // printf("%s\n",commands[i]);
            int arg_count = parse_command(commands[i], args);
            int opc = special_functions(args, PWD, OLDPWD,argc);

            bool background = false;
            if (strcmp(args[arg_count - 1], "&") == 0)
            {
                args[arg_count - 1] = NULL;
                background = true;
            }

            if (opc == -1)
            {
                break;
            }
            if (opc == 0)
            {
                continue;
            }

            create_fork(arg_count, args, background);

            i++;
        }
        return 0;
    }

    while (1)
    {
        read_command(cmd); // Get the command from user
        if (strlen(cmd) == 0)
            continue; // Skip empty input

        // Parse the command into program and arguments
        int arg_count = parse_command(cmd, args);

        // See if any of the arguments are within these
        int opc = special_functions(args, PWD, OLDPWD, arg_count);

        bool background = false;
        if (strcmp(args[arg_count - 1], "&") == 0)
        {
            args[arg_count - 1] = NULL;
            background = true;
        }

        if (opc == -1)
        {
            break;
        }
        if (opc == 0)
        {
            continue;
        }

        // Create a child process to execute the command
        create_fork(arg_count, args, background);
    }

    return 0;
}
