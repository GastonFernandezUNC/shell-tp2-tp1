#include "cjson_handler.h"

#define FILE_BUFFER_SIZE 1024
#define INITIAL_SIZE 1024
#define PATH_TO_CONFIG_FILE "../config.json"

char* _get_json()
{
    FILE* fp;
    char* content = NULL;
    size_t content_size = 0;
    char buffer[FILE_BUFFER_SIZE];
    // Opening file
    fp = fopen(PATH_TO_CONFIG_FILE, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    // Allocate initial memory for content
    content = malloc(INITIAL_SIZE);
    if (content == NULL)
    {
        perror("Memory allocation failed");
        fclose(fp);
        return -1;
    }
    content[0] = '\0'; // Initialize empty string

    // Read file line by line and append to content
    while (fgets(buffer, sizeof(buffer), fp))
    {
        size_t line_length = strlen(buffer);

        // Resize content buffer if necessary
        if (content_size + line_length + 1 > INITIAL_SIZE)
        {
            char* temp = realloc(content, content_size + line_length + 1);
            if (temp == NULL)
            {
                perror("Memory reallocation failed");
                free(content);
                fclose(fp);
                return -1;
            }
            content = temp;
        }

        // Append current line to content
        strcat(content, buffer);
        content_size += line_length;
    }

    fclose(fp);
    return content;
}

int save_to_config_file(cJSON* json) {
    // Serialize JSON to string
    char* updated_json = cJSON_Print(json);  // Use cJSON_PrintUnformatted() if you don't want pretty formatting.
    if (updated_json == NULL) {
        perror("Failed to serialize JSON");
        return EXIT_FAILURE;
    }

    // Open the file for writing
    FILE* fp = fopen(PATH_TO_CONFIG_FILE, "w");
    if (fp == NULL) {
        perror("Error opening file for writing");
        free(updated_json); // Free the serialized JSON string
        return EXIT_FAILURE;
    }

    // Write the JSON string to the file
    if (fputs(updated_json, fp) == EOF) {
        perror("Error writing JSON to file");
        fclose(fp);
        free(updated_json);
        return EXIT_FAILURE;
    }

    // Clean up
    fclose(fp);
    free(updated_json);
    return EXIT_SUCCESS;
}


int _write_json(cJSON* json, char* metric, bool option, int time) {
    if (time > 0) {
        cJSON* refresh_time = cJSON_GetObjectItem(json, "refresh_time");
        if (refresh_time) {
            cJSON_SetNumberValue(refresh_time, time);
        } else {
            cJSON_AddNumberToObject(json, "refresh_time", time);
        }
        return EXIT_SUCCESS;
    }

    if (metric != NULL) {
        cJSON* metrics = cJSON_GetObjectItem(json, "metrics");
        if (metrics) {
            cJSON* specific_metric = cJSON_GetObjectItem(metrics, metric);
            if (specific_metric && cJSON_IsObject(specific_metric)) {
                cJSON* enabled = cJSON_GetObjectItem(specific_metric, "enabled");
                if (enabled && cJSON_IsBool(enabled)) {
                    cJSON_SetBoolValue(enabled, option);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}


int config_monitor(char** args)
{

    char* content = _get_json();
    printf("File content:\n%s", content);
    
    if (args[1] == NULL || strcmp(args[1], "-D") != 0 && strcmp(args[1], "-d") != 0 && strcmp(args[1], "-A") != 0 &&
                               strcmp(args[1], "-a") != 0 && strcmp(args[1], "--help") != 0 &&
                               strcmp(args[1], "--h") != 0 && strcmp(args[1], "-n") != 0 && strcmp(args[1], "-N") != 0)
    {
        printf("Invalid argument\nValid arguments are\n-D -d: Deactivate\n-A -a: Activate\n"
               "-N -n: Set refresh time\n");
        return -1;
    }

    if (strcmp(args[1], "--help") == 0 || strcmp(args[1], "--h") == 0)
    {
        printf("Valid arguments are\n-D -d: Deactivate\n-A -a: Activate\n");
        printf("To activate or deactivate the of any metric, use the following format:\n"
               "config_monitor -d|-D|-a|-A <metric>\n");
        printf("Valid metrics are:\n"
               "   - cpu\n"
               "   - memory\n"
               "   - disk\n"
               "   - network\n"
               "   - proc_stats\n");

        return EXIT_SUCCESS;
    }

    char* metric = NULL;
    bool option = true;
    if (strcasecmp(args[1], "-d") == 0 || strcasecmp(args[1], "-a") == 0)
    {
        option = strcasecmp(args[1], "-a") == 0 ? true : false;
        if (strcasecmp(args[2], "cpu") == 0 || strcasecmp(args[2], "memory") == 0 || strcasecmp(args[2], "disk") == 0 ||
            strcasecmp(args[2], "network") == 0 || strcasecmp(args[2], "proc_stats") == 0)
        {
            metric = args[2];
        }
        else
        {
            printf("Invalid metric option\nCheck --help for valid options\n");
            return EXIT_FAILURE;
        }
    }

    int time = 0;
    if (strcasecmp(args[1], "-n") == 0 && args[2] != NULL)
    {
        time = atoi(args[2]);
        if (time <= 0)
        {
            printf("Invalid refresh time\n");
            return EXIT_FAILURE;
        }
    }


    cJSON* json = cJSON_Parse(content);
    if (json == NULL)
    {
        printf("Error parsing JSON\n");
        return -1;
    }

    _write_json(json, metric, option, time);
    save_to_config_file(json);

    free(content);
    cJSON_Delete(json);
    return EXIT_SUCCESS;
}