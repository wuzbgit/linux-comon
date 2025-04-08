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

extern void *command_server(void *arg);
extern int alg_process(struct CamFrame *frame);
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

    pthread_create(&cmd_thread, NULL, command_server, &dev);

    video_start_stream(&dev);
    
    gstream_loop();

    while(1) {
        usleep(10000000);
    }
    
    return 0;
}
