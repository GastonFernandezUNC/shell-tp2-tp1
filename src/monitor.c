#include "monitor.h"

void start_monitor(int* background_processes, pid_t* monitor)
{

    pid_t pid = fork();
    if (pid == 0)
    {

        char* args[] = {PATH_TO_METRICS, NULL};
        execvp(args[0], args);
    }
    else
    {
        printf("Monitor started\n");
        *background_processes += 1;
        printf("[%d] %d\n", *background_processes, pid);
        *monitor = pid;
    }
}

void stop_monitor(int* background_processes, pid_t* monitor)
{
    if (*monitor != -1)
    {
        *background_processes -= 1;
        kill(*monitor, SIGINT);
        *monitor = -1;
        printf("Monitor stopped\n");
    }
}

void read_fifo(void)
{
    char buffer[MAX_FIFO_BUFFER];
    int fd;
    ssize_t bytesRead;
    fd = open(PATH_TO_FIFO, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        char* stop_command = "status_stop";
        if (strncasecmp(buffer, stop_command, strlen(stop_command)) == 0)
            break;

        printf("%s", buffer);
    }

    if (bytesRead == -1)
    {
        perror("read");
    }
    close(fd);
}

void write_fifo(int* background_processes, pid_t* monitor)
{
    int fd;
    ssize_t bytesWritten;
    fd = open(PATH_TO_FIFO, O_WRONLY);
    if (fd == -1)
    {
        printf("ERROR PTH FIFO\n");
        if (mkfifo(PATH_TO_FIFO, FILE_PERMISSIONS) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
        write_fifo(background_processes, monitor);
        return;
    }

    char* message = "status\n";
    bytesWritten = write(fd, message, strlen(message));
    if (bytesWritten == -1)
    {
        perror("write");
    }

    close(fd);
}

void status_monitor(int* background_processes, pid_t* monitor)
{
    if (*monitor == -1)
    {
        printf("Monitor is not running\n");
    }
    else
    {
        write_fifo(background_processes, monitor);
        read_fifo();
    }
}
