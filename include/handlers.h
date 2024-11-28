#ifndef _HANDLERS_H
#define _HANDLERS_H

#include "cjson_handler.h"
#include "monitor.h"
#include <sys/wait.h>

#define _GNU_SOURCE
#define MAX_CMD_LEN 1024
#define MAX_CWD_BUFFER 1024

#define MAX_ARGS 100
#define FILE_BUFFER_SIZE 1024

#define CONTINUE 0
#define FORK 1
#define EXIT -1

void getCurrentPath(char* CWD, char* USER, char* HOSTNAME);

void handle_cd(char** args, char* PWD, char* OLDPWD);

int special_functions(char** args, char* PWD, char* OLDPWD, int* background_processes, int* monitor_pid);

void env_vars(char** args, int args_count);

void redir_function(char** args);

int check_redir(char** args);

int check_pipe(char** args);

void pipe_function(char** args, int command_count);

char** commands_file(char* file, int* lines_amount);

#endif
