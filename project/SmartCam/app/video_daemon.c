#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "gstreamer_pipe.h"
#include "ringbuffer.h"
#include "video.h"

#define WIDTH 1920
#define HEIGHT 1080
#define SOCKET_PATH "/tmp/video_control.sock"

extern int alg_process(struct CamFrame *frame);

void *command_listener(void *arg) {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[16];

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return NULL;
    }

    unlink(SOCKET_PATH);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return NULL;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return NULL;
    }

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer) - 1);
        
        if (strcmp(buffer, "start") == 0) {
            gstreamer_start_pipe();
        } else if (strcmp(buffer, "stop") == 0) {
            gstreamer_stop_pipe();
        }
        else if (strncmp(buffer, "resolution ", 11) == 0) {
            int new_width, new_height;
            if (sscanf(buffer + 11, "%dx%d", &new_width, &new_height) == 2) {
                gstreamer_set_pipeline_resolution(new_width, new_height);
            }
        }
        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return NULL;
}

extern void *net_stream_video(void *arg);

struct video_device dev;

int main(int argc, char *argv[])
{
    pthread_t cmd_thread, tcp_thread;
    int ret;
    
    ret = video_open(&dev);
    if (ret < 0) {
        printf("video open fail\n");
        return 0;
    }
     
    if (video_init(&dev, WIDTH, HEIGHT) < 0) {
        printf("can not set format width=%d height=%d\n",WIDTH,HEIGHT);
        return 0;
    }

    video_set_callback(alg_process);

    print_device_info(&dev);

    ringbuffer_init();

    gstreamer_create_pipeline(WIDTH, HEIGHT);
    // pthread_create(&tcp_thread, NULL, net_stream_video, &dev);

    pthread_create(&cmd_thread, NULL, command_listener, NULL);

    video_start_stream(&dev);
    
    gstream_loop();

    while(1) {
        usleep(10000000);
    }
    
    return 0;
}
