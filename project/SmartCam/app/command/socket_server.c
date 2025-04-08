#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "command_parser.h"
#include "../gstreamer_pipe.h"
#include "../video.h"


#define SOCKET_PATH "/tmp/cam.sock"

int create_unix_domain_server(const char *path)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
    unlink(path);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return -1;
    if (listen(fd, 5) < 0) return -1;

    return fd;
}

int accept_client(int server_fd)
{
    return accept(server_fd, NULL, NULL);
}

void *command_server(void *arg)
{
    struct video_device *dev = (struct video_device *)arg;
    int server_fd = create_unix_domain_server(SOCKET_PATH);
    if (server_fd < 0) {
        perror("Failed to create server socket");
        return 1;
    }

    while (1) {
        int client_fd = accept_client(server_fd);
        if (client_fd >= 0) {
            char buffer[1024] = {0};
            ssize_t len = recv(client_fd, buffer, sizeof(buffer)-1, 0);
            if (len > 0) {
                buffer[len] = '\0';
                handle_command(buffer, client_fd);
            }
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
