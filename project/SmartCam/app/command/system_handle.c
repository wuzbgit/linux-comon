#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cJSON/cJSON.h>
#include "command_parser.h"

void handle_system_command(cJSON *root, int client_fd)
{
    const char *module = "system";
    cJSON *action = cJSON_GetObjectItem(root, "action");

    if (!action || !cJSON_IsString(action)) {
        send_json_response(client_fd, module, NULL, NULL, "fail", NULL);
        return;
    }

    const char *act = action->valuestring;
    cJSON *resp_data = cJSON_CreateObject();

    if (strcmp(act, "get") == 0) {
        cJSON_AddStringToObject(resp_data, "ip", "192.168.1.100");
        cJSON_AddStringToObject(resp_data, "netmask", "255.255.255.0");
        cJSON_AddStringToObject(resp_data, "gateway", "192.168.1.1");
        send_json_response(client_fd, module, NULL, act, "success", resp_data);
    } else if (strcmp(act, "set") == 0) {
        cJSON *ip = cJSON_GetObjectItem(root, "ip");
        cJSON *netmask = cJSON_GetObjectItem(root, "netmask");
        cJSON *gateway = cJSON_GetObjectItem(root, "gateway");

        if (ip && netmask && gateway &&
            cJSON_IsString(ip) && cJSON_IsString(netmask) && cJSON_IsString(gateway)) {
            cJSON_AddItemToObject(resp_data, "ip", cJSON_Duplicate(ip, 1));
            cJSON_AddItemToObject(resp_data, "netmask", cJSON_Duplicate(netmask, 1));
            cJSON_AddItemToObject(resp_data, "gateway", cJSON_Duplicate(gateway, 1));
            send_json_response(client_fd, module, NULL, act, "success", resp_data);
        } else {
            send_json_response(client_fd, module, NULL, act, "fail", NULL);
        }
    } else {
        send_json_response(client_fd, module, NULL, act, "fail", NULL);
    }

    cJSON_Delete(resp_data);
}
