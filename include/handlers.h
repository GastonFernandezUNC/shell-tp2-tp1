#ifndef _HANDLERS_H
#define _HANDLERS_H

#include "cjson_handler.h"
#include "monitor.h"
#include <sys/wait.h>

#define _GNU_SOURCE

/**
 * @brief Define the maximum length of a command.
 */
#define MAX_CMD_LEN 1024

/**
 * @brief Define the maximum length of the current working directory.
 */
#define MAX_CWD_BUFFER 1024

/**
 * @brief Define the maximum number of arguments.
 */
#define MAX_ARGS 100

/**
 * @brief Define the size of the buffer used for storing data.
 */
#define FILE_BUFFER_SIZE 1024

/**
 * @brief Define the if to continue the while
 */
#define CONTINUE 0

/**
 * @brief Define the if to fork
 */
#define FORK 1

/**
 * @brief Define the if to exit
 */
#define EXIT -1

/**
 * @brief Define the if to do nothing
 */
#define NOTHING -2

/**
 * @brief Get the current directory, printing the user and hostname in the prompt.
 * @param CWD the current working directory.
 * @param USER the user.
 * @param HOSTNAME the hostname.
 */
void getCurrentPath(char* CWD, char* USER, char* HOSTNAME);

/**
 * @brief Handle the cd command. If it's '-' it changes to the previous directory.
 * @param args the arguments.
 * @param PWD the current working directory.
 * @param OLDPWD the old working directory.
 */
void handle_cd(char** args, char* PWD, char* OLDPWD);

/**
 * @brief Handle internal and other functions of the shell.
 * @param args the arguments.
 * @param PWD the current working directory.
 * @param OLDPWD the old working directory.
 * @param background_processes the background processes.
 * @param monitor_pid the monitor pid.
 * @return int
 */
int special_functions(char** args, char* PWD, char* OLDPWD, int* background_processes, int* monitor_pid);

/**
 * @brief Check if the arguments contain an environment variable.
 * @param args the argument.
 * @param args_count amount of arguments.
 */
void env_vars(char** args, int args_count);

/**
 * @brief Handles the use of the '<' and '>' operators.
 * @param args the arguments.
 */
void redir_function(char** args);

/**
 * @brief Check if the arguments contain the '<' and '>' operators.
 * @param args the arguments.
 * @return int
 */
int check_redir(char** args);

/**
 * @brief Check if the arguments contain the '|' operator.
 * @param args the arguments.
 * @return int
 */
int check_pipe(char** args);

/**
 * @brief Handles the use of the '|' operator.
 * @param args the arguments.
 * @param command_count the amount of commands.
 */
void pipe_function(char** args, int command_count);

/**
 * @brief Read the commands from a file.
 * @param file the file.
 * @param lines_amount the amount of lines.
 * @return char**
 */
char** commands_file(char* file, int* lines_amount);

#endif
