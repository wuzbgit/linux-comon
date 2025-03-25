#ifndef __CAMERA_H__
#define __CAMERA_H__

typedef struct CamFrame{
    char *buf;
    unsigned bytesperline;
    unsigned width;
    unsigned height;
    unsigned bytes;
} CamFrame_t;

typedef struct video_device {
	int fd;
	int type;
}video_device_t;

typedef int (*video_callback)(struct CamFrame *frame);


void print_device_info(struct video_device *dev);
int video_get_format(struct video_device *dev, unsigned *width, unsigned *height,unsigned *bytesperline, int *format);
int video_set_format(struct video_device *dev,unsigned width,unsigned height);
int video_set_framerate(struct video_device *dev, unsigned framerate);
int video_start_stream(struct video_device *dev);
int video_stop_stream(struct video_device *dev);
int video_open(struct video_device *dev);
int video_close(struct video_device *dev);
int video_set_callback(video_callback fn);
int video_deinit(struct video_device *dev, unsigned width, unsigned height);
int video_init(struct video_device *dev, unsigned width, unsigned height);
#endif
