#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "video.h"
#include "ringbuffer.h"

int alg_process(struct CamFrame *frame)
{
    ringbuffer_push(frame->buf, frame->width, frame->height, frame->bytesperline, frame->bytes);

    return 0;
}
