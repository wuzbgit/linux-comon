#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cJSON/cJSON.h>
#include "command_parser.h"

void handle_sensor_command(cJSON *root, int client_fd)
{
    const char *module = "sensor";
    cJSON *action = cJSON_GetObjectItem(root, "action");
    cJSON *submodule = cJSON_GetObjectItem(root, "submodule");

    if (!action || !cJSON_IsString(action)) {
        send_json_response(client_fd, module, NULL, NULL, "fail", NULL);
        return;
    }

    const char *act = action->valuestring;
    const char *sub = submodule && cJSON_IsString(submodule) ? submodule->valuestring : "";

    cJSON *resp_data = cJSON_CreateObject();

    // version 获取
    if (strcmp(sub, "version") == 0 && strcmp(act, "get") == 0) {
        cJSON_AddStringToObject(resp_data, "version", "v1.0.0");
        send_json_response(client_fd, module, sub, act, "success", resp_data);
    }

    // errorcode 获取
    else if (strcmp(sub, "errorcode") == 0 && strcmp(act, "get") == 0) {
        cJSON_AddNumberToObject(resp_data, "code", 0);
        send_json_response(client_fd, module, sub, act, "success", resp_data);
    }

    // framerate
    else if (strcmp(sub, "framerate") == 0) {
        if (strcmp(act, "get") == 0) {
            cJSON_AddStringToObject(resp_data, "fps", "30");
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else if (strcmp(act, "set") == 0) {
            cJSON *fps = cJSON_GetObjectItem(root, "fps");
            if (fps && cJSON_IsString(fps)) {
                cJSON_AddItemToObject(resp_data, "fps", cJSON_Duplicate(fps, 1));
                send_json_response(client_fd, module, sub, act, "success", resp_data);
            } else {
                send_json_response(client_fd, module, sub, act, "fail", NULL);
            }
        }
    }

    // exposure
    else if (strcmp(sub, "exposure") == 0) {
        cJSON *mode = cJSON_GetObjectItem(root, "mode");
        cJSON *value = cJSON_GetObjectItem(root, "value");

        if (strcmp(act, "get") == 0) {
            cJSON_AddStringToObject(resp_data, "mode", "manual");
            cJSON_AddStringToObject(resp_data, "value", "100");
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else if (strcmp(act, "set") == 0 && mode && value && cJSON_IsString(mode) && cJSON_IsString(value)) {
            cJSON_AddItemToObject(resp_data, "mode", cJSON_Duplicate(mode, 1));
            cJSON_AddItemToObject(resp_data, "value", cJSON_Duplicate(value, 1));
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else {
            send_json_response(client_fd, module, sub, act, "fail", NULL);
        }
    }

    // gain
    else if (strcmp(sub, "gain") == 0) {
        cJSON *mode = cJSON_GetObjectItem(root, "mode");
        cJSON *value = cJSON_GetObjectItem(root, "value");

        if (strcmp(act, "get") == 0) {
            cJSON_AddStringToObject(resp_data, "mode", "auto");
            cJSON_AddStringToObject(resp_data, "value", "50");
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else if (strcmp(act, "set") == 0 && mode && value && cJSON_IsString(mode) && cJSON_IsString(value)) {
            cJSON_AddItemToObject(resp_data, "mode", cJSON_Duplicate(mode, 1));
            cJSON_AddItemToObject(resp_data, "value", cJSON_Duplicate(value, 1));
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else {
            send_json_response(client_fd, module, sub, act, "fail", NULL);
        }
    }

    // brightness
    else if (strcmp(sub, "brightness") == 0) {
        cJSON *value = cJSON_GetObjectItem(root, "value");

        if (strcmp(act, "get") == 0) {
            cJSON_AddStringToObject(resp_data, "value", "128");
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else if (strcmp(act, "set") == 0 && value && cJSON_IsString(value)) {
            cJSON_AddItemToObject(resp_data, "value", cJSON_Duplicate(value, 1));
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else {
            send_json_response(client_fd, module, sub, act, "fail", NULL);
        }
    }

    // trigger
    else if (strcmp(sub, "trigger") == 0) {
        if (strcmp(act, "get") == 0) {
            cJSON_AddStringToObject(resp_data, "mode", "trigger");
            cJSON_AddStringToObject(resp_data, "trigger_number", "1");
            cJSON_AddStringToObject(resp_data, "trigger_delay", "100");
            cJSON_AddStringToObject(resp_data, "trigger_edge", "rising");
            cJSON_AddStringToObject(resp_data, "trigger_filter", "disable");
            cJSON_AddStringToObject(resp_data, "trigger_filter_width", "10");
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else if (strcmp(act, "set") == 0) {
            const char *fields[] = {
                "mode", "trigger_number", "trigger_delay",
                "trigger_edge", "trigger_filter", "trigger_filter_width"
            };

            int valid = 1;
            for (int i = 0; i < 6; i++) {
                cJSON *field = cJSON_GetObjectItem(root, fields[i]);
                if (!field || !cJSON_IsString(field)) {
                    valid = 0;
                    break;
                }
                cJSON_AddItemToObject(resp_data, fields[i], cJSON_Duplicate(field, 1));
            }

            if (valid) {
                send_json_response(client_fd, module, sub, act, "success", resp_data);
            } else {
                send_json_response(client_fd, module, sub, act, "fail", NULL);
            }
        } else {
            send_json_response(client_fd, module, sub, act, "fail", NULL);
        }
    }

    else {
        send_json_response(client_fd, module, sub, act, "fail", NULL);
    }

    cJSON_Delete(resp_data);
}
