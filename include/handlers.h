#ifndef _HANDLERS_H
#define _HANDLERS_H

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMD_LEN 1024
#define MAX_CWD_BUFFER 1024

#define MAX_ARGS 100

void getCurrentPath(char* CWD, char* USER, char* HOSTNAME);

void handle_cd(char** args, char* PWD, char* OLDPWD);

#endif
