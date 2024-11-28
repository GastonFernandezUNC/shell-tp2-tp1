#ifndef _MONITOR_H
#define _MONITOR_H

#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATH_TO_METRICS "so-i-24-GastonFernandezUNC/metrics"
#define PATH_TO_FIFO "/tmp/monitor_fifo"
#define MAX_FIFO_BUFFER 1024
#define FILE_PERMISSIONS 0666
#define METRICS_READING_DELAY 1

void start_monitor(int* background_processes, pid_t* monitor);
void stop_monitor(int* background_processes, pid_t* monitor);
void status_monitor(int* background_processes, pid_t* monitor);
void read_fifo(void);
void write_fifo(int* background_processes, pid_t* monitor);

#endif