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
#include <time.h>
#include "video.h"

#define VIDEO_DEVICE    "/dev/video0"
#define FRAME_BUFFER_COUNT 4

struct buffer {
	void *start;
	size_t length;
};
static struct buffer buffers[FRAME_BUFFER_COUNT];

static void fourcc_to_string(unsigned fourcc, char *str)
{
	str[0] = (fourcc & 0xFF);
	str[1] = (fourcc >> 8) & 0xFF;
	str[2] = (fourcc >> 16) & 0xFF;
	str[3] = (fourcc >> 24) & 0xFF;
	str[4] = '\0'; // Null-terminate the string
}

static const char *pixel_format_to_human_readable(unsigned fourcc)
{
	static char fmt_str[5];
	switch (fourcc) {
	case V4L2_PIX_FMT_YUYV:
		return "YUYV 4:2:2";
	case V4L2_PIX_FMT_MJPEG:
		return "MJPEG (Motion JPEG)";
	case V4L2_PIX_FMT_H264:
		return "H.264 (Advanced Video Coding)";
	case V4L2_PIX_FMT_YUV420:
		return "YUV 4:2:0";
	case V4L2_PIX_FMT_RGB24:
		return "RGB 24-bit";
	case V4L2_PIX_FMT_GREY:
		return "Greyscale (8-bit)";
	case V4L2_PIX_FMT_NV12:
		return "NV12 (YUV 4:2:0)";
	case V4L2_PIX_FMT_NV21:
		return "NV21 (YUV 4:2:0)";
	case V4L2_PIX_FMT_UYVY:
		return "UYVY 4:2:2";
	case V4L2_PIX_FMT_YUV422P:
		return "YUV 4:2:2 Planar";
	case V4L2_PIX_FMT_YUV444:
		return "YUV 4:4:4";
	default:
		fourcc_to_string(fourcc, fmt_str);
		return fmt_str;
	}
}

void print_device_capabilities(struct v4l2_capability cap)
{
	printf("Support capabilities:\n");

	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("V4L2_CAP_VIDEO_CAPTURE ");
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
		printf("V4L2_CAP_VIDEO_OUTPUT ");
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)) {
		printf("V4L2_CAP_VIDEO_OVERLAY ");
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY)) {
		printf("V4L2_CAP_VIDEO_OUTPUT_OVERLAY ");
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE)) {
		printf("V4L2_CAP_VIDEO_OUTPUT_MPLANE ");
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_M2M_MPLANE)) {
		printf("V4L2_CAP_VIDEO_M2M_MPLANE ");
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_M2M)) {
		printf("V4L2_CAP_VIDEO_M2M ");
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
		printf("V4L2_CAP_VIDEO_CAPTURE_MPLANE ");
	}

	if ((cap.capabilities & V4L2_CAP_EXT_PIX_FORMAT)) {
		printf("V4L2_CAP_EXT_PIX_FORMAT ");
	}

	printf("\n");
}

void enumerate_formats(struct video_device *dev)
{
	struct v4l2_fmtdesc fmt_desc;
	fmt_desc.index = 0;
	fmt_desc.type = dev->type;

	printf("Supported formats:\n");

	while (ioctl(dev->fd, VIDIOC_ENUM_FMT, &fmt_desc) == 0) {
		const char *format_name =
			pixel_format_to_human_readable(fmt_desc.pixelformat);
		printf("Index: %u, Format: %s\n", fmt_desc.index, format_name);
		fmt_desc.index++;
	}
}

static void print_cur_fmt(struct video_device *dev)
{
	struct v4l2_format fmt;

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = dev->type;

	if (ioctl(dev->fd, VIDIOC_G_FMT, &fmt) == -1) {
		perror("Error getting format");
		return;
	}
	const char *format_name =
		pixel_format_to_human_readable(fmt.fmt.pix_mp.pixelformat);
	printf("Width: %u\n", fmt.fmt.pix_mp.width);
	printf("Height: %u\n", fmt.fmt.pix_mp.height);
	printf("Pixel Format %s: %c%c%c%c\n", format_name,
	       fmt.fmt.pix_mp.pixelformat & 0xFF,
	       (fmt.fmt.pix_mp.pixelformat >> 8) & 0xFF,
	       (fmt.fmt.pix_mp.pixelformat >> 16) & 0xFF,
	       (fmt.fmt.pix_mp.pixelformat >> 24) & 0xFF);
	printf("Field: %u\n", fmt.fmt.pix_mp.field);
	printf("Number of Planes: %u\n", fmt.fmt.pix_mp.num_planes);

	for (unsigned int i = 0; i < fmt.fmt.pix_mp.num_planes; i++) {
		printf("Plane %u - Bytes Per Line: %u, Size Image: %u\n", i,
		       fmt.fmt.pix_mp.plane_fmt[i].bytesperline,
		       fmt.fmt.pix_mp.plane_fmt[i].sizeimage);
	}
}

void print_device_info(struct video_device *dev)
{
	struct v4l2_capability cap;

	if (ioctl(dev->fd, VIDIOC_QUERYCAP, &cap) == -1) {
		perror("Failed to query device capabilities");
		return;
	}

	printf("Device Info:\n");
	printf("  Driver: %s\n", cap.driver);
	printf("  Card: %s\n", cap.card);
	printf("  Bus Info: %s\n", cap.bus_info);
	printf("  Version: %u.%u.%u\n", (cap.version >> 16) & 0xFF,
	       (cap.version >> 8) & 0xFF, cap.version & 0xFF);
	print_device_capabilities(cap);
	enumerate_formats(dev);

	print_cur_fmt(dev);
}

static int get_video_type(int video_fd)
{
	struct v4l2_capability cap;
    int type = -1;

	if (ioctl(video_fd, VIDIOC_QUERYCAP, &cap) == -1) {
		printf("%s,%d:Not a valid video device\n",__func__,__LINE__);
		return -1;
	}

	if (!(cap.capabilities &
	      (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VIDEO_CAPTURE_MPLANE))) {
		fprintf(stderr, "Device does not support video capture: 0x%x\n",
			cap.capabilities);
		return -1;
	}
	if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	else
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	return type;
}

int video_set_format(struct video_device *dev,unsigned width,unsigned height)
{
    struct v4l2_format fmt;
    char cmd[128] = { 0 };

	snprintf(
		cmd, sizeof(cmd),
		"media-ctl --set-v4l2 '\"m00_b_mvcam 0-003b\":0[fmt:Y8_1X8/%dx%d]'",
		width, height);
	if (system(cmd) != 0) {
        perror("set camera format failed\n");

        return -1;
    }

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = dev->type;
	fmt.fmt.pix_mp.width = width;
	fmt.fmt.pix_mp.height = height;
	fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_GREY;
	fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;
	fmt.fmt.pix_mp.num_planes = 1;

	for (int i = 0; i < fmt.fmt.pix_mp.num_planes; i++) {
		fmt.fmt.pix_mp.plane_fmt[i].bytesperline = 0;
		fmt.fmt.pix_mp.plane_fmt[i].sizeimage = width * height;
	}
	if (ioctl(dev->fd, VIDIOC_S_FMT, &fmt) == -1) {
		perror("Failed to set format");
		return -1;
	}

	return 0;	
}

int video_set_framerate(struct video_device *dev, unsigned framerate)
{
    return 0;
}

int video_get_format(struct video_device *dev, unsigned *width, unsigned *height,unsigned *bytesperline, int *format)
{
	struct v4l2_format fmt;

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = dev->type;

	if (ioctl(dev->fd, VIDIOC_G_FMT, &fmt) == -1) {
		perror("Error getting format");
		return -1;
	}

	*width = fmt.fmt.pix_mp.width;
	*height = fmt.fmt.pix_mp.height;
	*format = fmt.fmt.pix_mp.pixelformat;
	*bytesperline = fmt.fmt.pix_mp.plane_fmt[0].bytesperline;
	return 0;	
}

int video_open(struct video_device *dev)
{
	dev->fd = open(VIDEO_DEVICE, O_RDWR);
	if (dev->fd < 0) {
		perror("open: ");
		return -1;
	}

	dev->type = get_video_type(dev->fd);
	if (dev->type < 0) {
		printf("video open failed : get video type failed\n");

		close(dev->fd);

		return -1;
	}

    return 0;
}

int video_close(struct video_device *dev)
{
    close(dev->fd);
}

int video_init(struct video_device *dev, unsigned width, unsigned height)
{
	struct v4l2_plane planes[VIDEO_MAX_PLANES] = { 0 };
	struct v4l2_buffer buf = { 0 };
	int i = 0;
	struct v4l2_requestbuffers req;
    struct CamFrame frame;

	video_set_format(dev,width,height);

	memset(&req, 0, sizeof(req));
	req.count = FRAME_BUFFER_COUNT;
	req.type = dev->type;
	req.memory = V4L2_MEMORY_MMAP;

	if (ioctl(dev->fd, VIDIOC_REQBUFS, &req) == -1) {
		perror("Failed to request buffers");
		return -1;
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i) {
		memset(&buf, 0, sizeof buf);
		memset(planes, 0, sizeof planes);
		buf.type = dev->type;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.length = VIDEO_MAX_PLANES;
		buf.m.planes = planes;
		buf.index = i;

		if (ioctl(dev->fd, VIDIOC_QUERYBUF, &buf) == -1) {
			perror("Failed to query buffer");
			return -1;
		}

		buffers[i].length = buf.m.planes[0].length;
		buffers[i].start = mmap(NULL, buffers[i].length,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					dev->fd, buf.m.planes[0].m.mem_offset);

		if (buffers[i].start == MAP_FAILED) {
			perror("Failed to map buffer");
			return -1;
		}

		if (ioctl(dev->fd, VIDIOC_QBUF, &buf) == -1) {
			perror("Failed to queue buffer");
			return -1;
		}
	}

	return 0;
}

int video_deinit(struct video_device *dev, unsigned width, unsigned height)
{
	int i;
	for (i = 0; i < FRAME_BUFFER_COUNT; ++i) {
		munmap(buffers[i].start, buffers[i].length);
	}

	close(dev->fd);
}

static pthread_t video_thread;
static int loop = 0;
static video_callback callback = NULL;

int video_set_callback(video_callback fn)
{
	callback = fn;
}

static void *video_thread_loop(void *arg)
{
	struct v4l2_plane planes[VIDEO_MAX_PLANES] = { 0 };
	struct v4l2_buffer buf = { 0 };
    struct CamFrame frame;
	struct video_device *dev = (struct video_device *)arg;
	int format;

	video_get_format(dev, &frame.width , &frame.height, &frame.bytesperline, &format);

	while (loop) {
		memset(&buf, 0, sizeof(buf));
		memset(planes, 0, sizeof planes);
		buf.type = dev->type;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.length = VIDEO_MAX_PLANES;
		buf.m.planes = planes;
		
		if (ioctl(dev->fd, VIDIOC_DQBUF, &buf) == -1) {
			perror("Failed to dequeue buffer");
			break;
		}

        frame.buf = (char *)buffers[buf.index].start;
        frame.bytes = buf.m.planes[0].bytesused;
		// printf("Captured frame (size: %u bytes)\n", buf.m.planes[0].bytesused);
		if (callback) {
			callback(&frame);
		}
        
		if (ioctl(dev->fd, VIDIOC_QBUF, &buf) == -1) {
			perror("Failed to requeue buffer");
			break;
		}
	}

    return 0; 
}

int video_start_stream(struct video_device *dev)
{
    enum v4l2_buf_type buf_type;

	buf_type = dev->type;

	if (ioctl(dev->fd, VIDIOC_STREAMON, &buf_type) == -1) {
		perror("Failed to start streaming");
		return -1;
	}

	loop = 1;
	
	pthread_create(&video_thread, NULL, video_thread_loop, dev);

    return 0;
}

int video_stop_stream(struct video_device *dev)
{
    int type = -1;
    enum v4l2_buf_type buf_type = dev->type;

	if (ioctl(dev->fd, VIDIOC_STREAMOFF, &buf_type) == -1) {
		perror("Failed to stop streaming");

        return -1;
	}

	loop = 0;

	pthread_join(video_thread, NULL);

    return 0;
}
