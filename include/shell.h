#ifndef _SHELL_H
#define _SHELL_H

#include "handlers.h"
#include <stdbool.h>

void read_command(char* cmd, char* CWD, char* USER, char* HOSTNAME);
int parse_command(char* cmd, char** args);
void create_fork(char** args, bool background, int* current_child, int* background_processes);
void sig_handler(int signo, int* current_child);
#endif
