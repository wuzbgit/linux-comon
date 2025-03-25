#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "video.h"
#include "ringbuffer.h"

#define SERVER_PORT 8500
#define MAX_WIDTH   4096
#define MAX_HEIGHT  4096

typedef struct {
    int width;
    int height;
    int format;
	int size;	
} FrameHeader;

void stream_video(struct video_device *dev, int client_fd)
{
    char *packet = (char *)malloc(sizeof(FrameHeader) + MAX_WIDTH*MAX_HEIGHT);
	char *buffer = (char *)malloc(sizeof(FrameHeader) + MAX_WIDTH*MAX_HEIGHT);
    unsigned width,height;
    FrameHeader header;
	frame_item *frame;
	unsigned bytesperline = 0;
	int i;
	
    video_get_format(dev, &header.width , &header.height, &bytesperline, &header.format);
	header.size = header.width * header.height;
    printf("current video width=%d height=%d format=0x%x\n",header.width, header.height, header.format);
    video_start_stream(dev);
	while (1) {

        if (!(frame = ringbuffer_pop(&frame))) {
            printf("gstreamer can not get frame from ringbuffer\n");
            continue ;
        }

		// printf("Get frame width=%d,height=%d,size=%d\n",frame.width, frame.height, frame.size);
		memcpy(packet, &header, sizeof(FrameHeader));
		
		for (i = 0; i < header.height; i++) {
			memcpy(packet + sizeof(FrameHeader) + i * header.height, frame->buf + i * bytesperline, header.width);
		}
		memcpy(packet + sizeof(FrameHeader), frame->buf, frame->size);
        if (send(client_fd, packet, sizeof(FrameHeader) + frame->size, 0) < 0) {
            perror("send packet");
            break;
        }
        ringbuffer_release(frame);
		if (recv(client_fd, buffer, 100, 0) < 0) {
        	perror("recv ack");
        	break;
        }
	}

	free(packet);
	free(buffer);
	printf("Client disconnected.\n");
}

void *net_stream_video(void *arg)
{
	int server_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);
    struct video_device *dev = (struct video_device *)arg;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("Failed to create socket");
		pthread_exit(NULL);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT);

	if (bind(server_fd, (struct sockaddr *)&server_addr,
		 sizeof(server_addr)) == -1) {
		perror("Failed to bind socket");
		close(server_fd);
		pthread_exit(NULL);
	}

	if (listen(server_fd, 1) == -1) {
		perror("Failed to listen on socket");
		close(server_fd);
		pthread_exit(NULL);
	}

	printf("Server listening on port %d...\n", SERVER_PORT);
    while (1) {
	    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
	    if (client_fd == -1) {
		    perror("Failed to accept connection");
		    close(server_fd);
		    pthread_exit(NULL);
	    }
        printf("Client connected. Streaming video...\n");

        stream_video(dev,client_fd);

        close(client_fd); 
    }
	
	close(server_fd);

    pthread_exit(NULL);
}
