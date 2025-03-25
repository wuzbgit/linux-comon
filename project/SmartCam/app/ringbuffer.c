#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "video.h"
#include "ringbuffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "video.h"
#include "ringbuffer.h"

#define MAX_BUFFERS 5
#define MAX_WIDTH  4096
#define MAX_HEIGHT 4096

typedef struct {
    frame_item *items[MAX_BUFFERS];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} queue_t;

static queue_t free_queue = { .head = 0, .tail = 0, .count = 0, .lock = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER };
static queue_t used_queue = { .head = 0, .tail = 0, .count = 0, .lock = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER };
static frame_item buffers[MAX_BUFFERS];

static void queue_push(queue_t *queue, frame_item *item)
{
    pthread_mutex_lock(&queue->lock);
    while (queue->count >= MAX_BUFFERS) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    queue->items[queue->tail] = item;
    queue->tail = (queue->tail + 1) % MAX_BUFFERS;
    queue->count++;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

static frame_item *queue_pop(queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);
    while (queue->count == 0) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    frame_item *item = queue->items[queue->head];
    queue->head = (queue->head + 1) % MAX_BUFFERS;
    queue->count--;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
    return item;
}

void ringbuffer_init()
{
    for (int i = 0; i < MAX_BUFFERS; i++)
    {
        buffers[i].buf = malloc(MAX_WIDTH * MAX_HEIGHT * 3); // 3 bytes per pixel (RGB)
        queue_push(&free_queue, &buffers[i]);
    }
}

void ringbuffer_release(frame_item *frame)
{
    queue_push(&free_queue, frame);
}

void ringbuffer_push(void *buf, unsigned width, unsigned height, unsigned bytesperline,unsigned size)
{
    frame_item *frame = queue_pop(&free_queue);
    int i;
    if (frame) {
        frame->width = width;
        frame->height = height;
        frame->size = width*height;
        if (bytesperline == width) {
            memcpy(frame->buf, buf, size);
        }
        else {
            for (i = 0; i < height; i++) {
                memcpy(frame->buf + i * width, buf + i * bytesperline, width);
            }
        }

        queue_push(&used_queue, frame);
    }
}

frame_item *ringbuffer_pop()
{
    frame_item *frame = queue_pop(&used_queue);

    return frame;
}
