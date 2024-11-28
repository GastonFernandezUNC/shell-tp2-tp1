#ifndef _CJSON_HANDLER_H
#define _CJSON_HANDLER_H

#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define FAILED_EXIT "FAILED_EXIT"
#define FILE_BUFFER_SIZE 1024
#define INITIAL_SIZE 1024
#define PATH_TO_CONFIG_FILE "../config.json"

int config_monitor(char** args);
char* _get_json(void);
int _write_json(cJSON* json, char* metric, bool option, int time);
int save_to_config_file(cJSON* json);
#endif