#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#define RING_BUFFER_SIZE    20
#define MAX_WIDTH           4096
#define MAX_HEIGHT          4096

typedef struct {
    char *buf;
    unsigned max_size;
    unsigned width;
    unsigned height;
    unsigned size;
} frame_item;

void ringbuffer_init();

void ringbuffer_release(frame_item *frame);

void ringbuffer_push(void *buf, unsigned width, unsigned height, unsigned bytesperline,unsigned size);

frame_item *ringbuffer_pop();

#endif