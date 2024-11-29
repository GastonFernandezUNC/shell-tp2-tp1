#ifndef _SHELL_H
#define _SHELL_H

#include "handlers.h"
#include <stdbool.h>

/**
 * @brief Reads the command from the console.
 * @param cmd the command.
 * @param CWD the current working directory.
 * @param USER the user.
 * @param HOSTNAME the hostname.
 */
void read_command(char* cmd, char* CWD, char* USER, char* HOSTNAME);

/**
 * @brief Parses the command into program and arguments.
 * @param cmd the command.
 * @param args the arguments.
 * @return int
 */
int parse_command(char* cmd, char** args);

/**
 * @brief Creates a fork to execute the command.
 * @param args the arguments.
 * @param background if the process is in the background.
 * @param current_child the current child process.
 * @param background_processes the background processes.
 */
void create_fork(char** args, bool background, int* current_child, int* background_processes);

/**
 * @brief Handles the shell.
 * @param signo the signal number.
 * @param current_child the current child process.
 */
void sig_handler(int signo, int* current_child);

/**
 * @brief Sets the path to the configuration file.
 */
void set_config_path(void);
#endif
