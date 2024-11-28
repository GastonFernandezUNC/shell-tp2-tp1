#include "shell.h"

// Main function
int main(int argc, char* argv[])
{

    char CWD[MAX_CWD_BUFFER], PWD[MAX_CWD_BUFFER], OLDPWD[MAX_CWD_BUFFER];
    char *USER, HOSTNAME[MAX_CMD_LEN];
    int background_processes = 0;
    int current_child = -1;
    bool stop_requested = false;
    pid_t monitor_pid = -1;

    signal(SIGINT,  sig_handler);
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
            int opc = special_functions(args, PWD, OLDPWD, &background_processes, &monitor_pid);

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

            create_fork(args, background, &current_child, &background_processes);

            i++;
        }
        return 0;
    }

    while (1)
    {
        read_command(cmd, CWD, USER, HOSTNAME); // Get the command from user
        if (strlen(cmd) == 0)
            continue; // Skip empty input

        // Parse the command into program and arguments
        int arg_count = parse_command(cmd, args);

        // See if any of the arguments are within these
        int opc = special_functions(args, PWD, OLDPWD, &background_processes, &monitor_pid);

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

        int pipes_count = check_pipe(args);
        if (pipes_count > 0)
        {
            pipe_function(args, pipes_count);
            continue;
        }

        // Create a child process to execute the command
        create_fork(args, background, &current_child, &background_processes);
    }

    return 0;
}
