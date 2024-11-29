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

/**
 * @brief Path to the metrics bin.
 */
#define PATH_TO_METRICS "./metrics-tp1-so1/metrics"

/**
 * @brief Path to the fifo.
 */
#define PATH_TO_FIFO "/tmp/monitor_fifo"

/**
 * @brief Maximum size of the buffer used for storing data for the FIFO.
 */
#define MAX_FIFO_BUFFER 1024

/**
 * @brief File permissions for the FIFO.
 */
#define FILE_PERMISSIONS 0666

/**
 * @brief Metrics reading delay.
 */
#define METRICS_READING_DELAY 1

/**
 * @brief Start the monitor.
 * @param background_processes the background processes.
 * @param monitor the monitor pid.
 */
void start_monitor(int* background_processes, pid_t* monitor);

/**
 * @brief Stop the monitor.
 * @param background_processes the background processes.
 * @param monitor the monitor pid.
 */
void stop_monitor(int* background_processes, pid_t* monitor);

/**
 * @brief Prints the selected info collected from the json config file.
 * @param background_processes the background processes.
 * @param monitor the monitor pid.
 */
void status_monitor(int* background_processes, pid_t* monitor);

/**
 * @brief Read the fifo.
 */
void read_fifo(void);

/**
 * @brief Write the fifo.
 * @param background_processes the background processes.
 * @param monitor the monitor pid.
 */
void write_fifo(int* background_processes, pid_t* monitor);

#endif
