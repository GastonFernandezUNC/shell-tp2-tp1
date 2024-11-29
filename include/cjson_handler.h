#ifndef _CJSON_HANDLER_H
#define _CJSON_HANDLER_H

#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define PATH_TO_CONFIG_FILE "../config.json"

/**
 * @brief Define failed exit.
 */
#define FAILED_EXIT "FAILED_EXIT"

/**
 * @brief Define buffer size.
 */
#define FILE_BUFFER_SIZE 1024

/**
 * @brief Define initial size.
 */
#define INITIAL_SIZE 1024

/**
 * @brief Function to configure what metrics will be shown by the status_monitor command
 * @param args what will be done, use --help or --h to see the whole config
 * @return int
 */
int config_monitor(char** args);

/**
 * @brief Function to get the json from the config file as a string
 * @return char*
 */
char* _get_json(void);

/**
 * @brief Function to write the json to the config file
 * @param json the json to be written
 * @param metric the metric to be updated
 * @param option the option to be set
 * @param time the time to be set
 * @return int
 */
int _write_json(cJSON* json, char* metric, bool option, int time);

/**
 * @brief Function to save the json from memory into the file
 * @param json the json to be saved
 * @return int
 */
int save_to_config_file(cJSON* json);
#endif
