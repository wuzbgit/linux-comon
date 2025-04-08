#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cJSON/cJSON.h>
#include "command_parser.h"

void handle_video_command(cJSON *root, int client_fd);
void handle_sensor_command(cJSON *root, int client_fd);
void handle_system_command(cJSON *root, int client_fd);

void send_json_response(int fd, const char *module, const char *submodule, const char *action, const char *state, cJSON *data)
{
    cJSON *resp = cJSON_CreateObject();

    cJSON_AddStringToObject(resp, "module", module);
    if (submodule) cJSON_AddStringToObject(resp, "submodule", submodule);
    if (action) cJSON_AddStringToObject(resp, "action", action);
    cJSON_AddStringToObject(resp, "state", state);

    if (data) {
        cJSON *child = data->child;
        while (child) {
            cJSON_AddItemToObject(resp, child->string, cJSON_Duplicate(child, 1));
            child = child->next;
        }
    }

    char *out = cJSON_PrintUnformatted(resp);
    if (out) {
        write(fd, out, strlen(out));
        write(fd, "\n", 1);
        free(out);
    }

    cJSON_Delete(resp);
}

void handle_command(const char *msg, int client_fd)
{
    cJSON *root = cJSON_Parse(msg);
    if (!root) {
        send_json_response(client_fd, "unknown", NULL, NULL, "fail", NULL);
        return;
    }

    cJSON *module = cJSON_GetObjectItem(root, "module");
    cJSON *action = cJSON_GetObjectItem(root, "action");

    if (!module || !cJSON_IsString(module) || !action || !cJSON_IsString(action)) {
        send_json_response(client_fd, "unknown", NULL, NULL, "fail", NULL);
        cJSON_Delete(root);
        return;
    }

    const char *mod = module->valuestring;

    if (strcmp(mod, "sensor") == 0) {
        handle_sensor_command(root, client_fd);
    } else if (strcmp(mod, "video") == 0) {
        handle_video_command(root, client_fd);
    } else if (strcmp(mod, "system") == 0) {
        handle_system_command(root, client_fd);
    } else {
        cJSON *sub = cJSON_GetObjectItem(root, "submodule");
        const char *sub_val = (sub && cJSON_IsString(sub)) ? sub->valuestring : NULL;
        send_json_response(client_fd, mod, sub_val, action->valuestring, "fail", NULL);
    }

    cJSON_Delete(root);
}
