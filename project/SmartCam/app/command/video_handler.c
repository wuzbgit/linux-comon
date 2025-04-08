#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cJSON/cJSON.h>
#include "command_parser.h"

void handle_video_command(cJSON *root, int client_fd) {
    const char *module = "video";
    cJSON *action = cJSON_GetObjectItem(root, "action");
    cJSON *submodule = cJSON_GetObjectItem(root, "submodule");

    if (!action || !cJSON_IsString(action)) {
        send_json_response(client_fd, module, NULL, NULL, "fail", NULL);
        return;
    }

    const char *act = action->valuestring;
    const char *sub = submodule && cJSON_IsString(submodule) ? submodule->valuestring : "";

    cJSON *resp_data = cJSON_CreateObject();

    if (strcmp(sub, "framerate") == 0) {
        if (strcmp(act, "get") == 0) {
            cJSON_AddStringToObject(resp_data, "fps", "25");
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

    else if (strcmp(sub, "format") == 0) {
        if (strcmp(act, "get") == 0) {
            cJSON_AddStringToObject(resp_data, "format", "H264");
            send_json_response(client_fd, module, sub, act, "success", resp_data);
        } else if (strcmp(act, "set") == 0) {
            cJSON *fmt = cJSON_GetObjectItem(root, "format");
            if (fmt && cJSON_IsString(fmt)) {
                cJSON_AddItemToObject(resp_data, "format", cJSON_Duplicate(fmt, 1));
                send_json_response(client_fd, module, sub, act, "success", resp_data);
            } else {
                send_json_response(client_fd, module, sub, act, "fail", NULL);
            }
        }
    }

    else if (strcmp(sub, "stream") == 0) {
        if (strcmp(act, "start") == 0 || strcmp(act, "stop") == 0) {
            send_json_response(client_fd, module, sub, act, "success", NULL);
        } else {
            send_json_response(client_fd, module, sub, act, "fail", NULL);
        }
    }

    else {
        send_json_response(client_fd, module, sub, act, "fail", NULL);
    }

    cJSON_Delete(resp_data);
}
