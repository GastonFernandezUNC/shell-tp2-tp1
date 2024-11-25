#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cjson/cJSON.h>

int config_monitor(char** args);
char* _get_json();
int _write_json(cJSON* json, char* metric, bool option, int time);



