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
#include <time.h>

#define WIDTH 2048
#define HEIGHT 1080
#define FORMAT V4L2_PIX_FMT_GREY
#define NUM_BUFFERS 4
#define SERVER_PORT 8500
#define MAX_CLIENTS 5

#define FRAME_BUFFER_COUNT 4

struct buffer {
	void *start;
	size_t length;
};

struct device {
	int fd;
	int type;
};

void print_usage(const char *prog_name)
{
	printf("Usage: %s --device <video_device> --cmd <info|stream>\n",
	       prog_name);
}

void calculate_fps() {
    static struct timespec start = {0};
    static unsigned int frame_count = 0;
    struct timespec current;
    
    if (start.tv_sec == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        return;
    }
    
    frame_count++;
    
    clock_gettime(CLOCK_MONOTONIC, &current);
    
    long elapsed_ms = (current.tv_sec - start.tv_sec) * 1000 + 
                     (current.tv_nsec - start.tv_nsec) / 1000000;
    
    if (elapsed_ms >= 1000) {
        double fps = frame_count / (elapsed_ms / 1000.0);
        printf("FPS: %.2f\n", fps);
        
        frame_count = 0;
        start = current;
    }
}

int is_video_device(struct device *dev)
{
	struct v4l2_capability cap;

	if (ioctl(dev->fd, VIDIOC_QUERYCAP, &cap) == -1) {
		perror("Not a valid video device");
		return 0;
	}
	if (!(cap.capabilities &
	      (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VIDEO_CAPTURE_MPLANE))) {
		fprintf(stderr, "Device does not support video capture: 0x%x\n",
			cap.capabilities);
		return 0;
	}
	if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
		dev->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	else
		dev->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	return 1;
}

void fourcc_to_string(unsigned fourcc, char *str)
{
	str[0] = (fourcc & 0xFF);
	str[1] = (fourcc >> 8) & 0xFF;
	str[2] = (fourcc >> 16) & 0xFF;
	str[3] = (fourcc >> 24) & 0xFF;
	str[4] = '\0'; // Null-terminate the string
}

const char *pixel_format_to_human_readable(unsigned fourcc)
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

void enumerate_formats(struct device *dev)
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

void print_cur_fmt(struct device *dev)
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

void print_device_info(struct device *dev)
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

void stream_video(struct device *dev)
{
	struct v4l2_format fmt;

	char cmd[128] = { 0 };
	snprintf(
		cmd, sizeof(cmd),
		"media-ctl --set-v4l2 '\"m00_b_mvcam 0-003b\":0[fmt:Y8_1X8/%dx%d]'",
		WIDTH, HEIGHT);
	system(cmd);

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = dev->type;
	fmt.fmt.pix_mp.width = WIDTH;
	fmt.fmt.pix_mp.height = HEIGHT;
	fmt.fmt.pix_mp.pixelformat = FORMAT;
	fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;
	fmt.fmt.pix_mp.num_planes = 1;

	for (int i = 0; i < fmt.fmt.pix_mp.num_planes; i++) {
		fmt.fmt.pix_mp.plane_fmt[i].bytesperline = 0;
		fmt.fmt.pix_mp.plane_fmt[i].sizeimage = WIDTH * HEIGHT;
	}
	if (ioctl(dev->fd, VIDIOC_S_FMT, &fmt) == -1) {
		perror("Failed to set format");
		return;
	}
	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
	req.count = FRAME_BUFFER_COUNT;
	req.type = dev->type;
	req.memory = V4L2_MEMORY_MMAP;

	if (ioctl(dev->fd, VIDIOC_REQBUFS, &req) == -1) {
		perror("Failed to request buffers");
		return;
	}

	struct buffer buffers[FRAME_BUFFER_COUNT];
	struct v4l2_plane planes[VIDEO_MAX_PLANES] = { 0 };
	struct v4l2_buffer buf = { 0 };

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
			return;
		}

		buffers[i].length = buf.m.planes[0].length;
		buffers[i].start = mmap(NULL, buffers[i].length,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					dev->fd, buf.m.planes[0].m.mem_offset);

		if (buffers[i].start == MAP_FAILED) {
			perror("Failed to map buffer");
			return;
		}

		if (ioctl(dev->fd, VIDIOC_QBUF, &buf) == -1) {
			perror("Failed to queue buffer");
			return;
		}
	}

	enum v4l2_buf_type type = dev->type;
	if (ioctl(dev->fd, VIDIOC_STREAMON, &type) == -1) {
		perror("Failed to start streaming");
		return;
	}

	printf("Streaming video\n");
	int i = 0;
	while (1) {
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

		buf.bytesused = buf.m.planes[0].bytesused;
		// printf("Captured frame %d (size: %u bytes)\n", i++,
		//        buf.bytesused);

		if (ioctl(dev->fd, VIDIOC_QBUF, &buf) == -1) {
			perror("Failed to requeue buffer");
			break;
		}

		calculate_fps();
		// usleep(100000); // Simulate 100ms delay
	}

	if (ioctl(dev->fd, VIDIOC_STREAMOFF, &type) == -1) {
		perror("Failed to stop streaming");
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i) {
		munmap(buffers[i].start, buffers[i].length);
	}

	printf("Stopped streaming.\n");
}

static int set_stream_format(struct device *dev,unsigned width,unsigned height)
{
	struct v4l2_format fmt;
	char cmd[128] = { 0 };
	snprintf(
		cmd, sizeof(cmd),
		"media-ctl --set-v4l2 '\"m00_b_mvcam 0-003b\":0[fmt:Y8_1X8/%dx%d]'",
		width, height);
	system(cmd);

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = dev->type;
	fmt.fmt.pix_mp.width = width;
	fmt.fmt.pix_mp.height = height;
	fmt.fmt.pix_mp.pixelformat = FORMAT;
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

typedef struct {
    int width;
    int height;
    int format;
	int size;
} FrameHeader;

void net_stream_video(struct device *dev)
{
	struct v4l2_format fmt;
	FrameHeader header;

    set_stream_format(dev,WIDTH,HEIGHT);
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(dev->fd, VIDIOC_G_FMT, &fmt) < 0) {
        perror("get video format");
        return;
    }

    printf("Video width: %d, height: %d, format[0x%x]: %c%c%c%c\n",
           fmt.fmt.pix.width, fmt.fmt.pix.height,
		   fmt.fmt.pix.pixelformat,
           (fmt.fmt.pix.pixelformat & 0xff),
           ((fmt.fmt.pix.pixelformat >> 8) & 0xff),
           ((fmt.fmt.pix.pixelformat >> 16) & 0xff),
           ((fmt.fmt.pix.pixelformat >> 24) & 0xff));

    header.width = fmt.fmt.pix.width;
    header.height = fmt.fmt.pix.height;
    header.format = fmt.fmt.pix.pixelformat;

	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
	req.count = FRAME_BUFFER_COUNT;
	req.type = dev->type;
	req.memory = V4L2_MEMORY_MMAP;

	if (ioctl(dev->fd, VIDIOC_REQBUFS, &req) == -1) {
		perror("Failed to request buffers");
		return;
	}

	struct buffer buffers[FRAME_BUFFER_COUNT];
	struct v4l2_plane planes[VIDEO_MAX_PLANES] = { 0 };
	struct v4l2_buffer buf;

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
			return;
		}

		buffers[i].length = buf.m.planes[0].length;
		buffers[i].start = mmap(NULL, buffers[i].length,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					dev->fd, buf.m.planes[0].m.mem_offset);
		buffers[i].length = buf.length;

		if (buffers[i].start == MAP_FAILED) {
			perror("Failed to map buffer");
			return;
		}

		if (ioctl(dev->fd, VIDIOC_QBUF, &buf) == -1) {
			perror("Failed to queue buffer");
			return;
		}
	}

	enum v4l2_buf_type type = dev->type;
	if (ioctl(dev->fd, VIDIOC_STREAMON, &type) == -1) {
		perror("Failed to start streaming");
		return;
	}

	int server_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("Failed to create socket");
		return;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT);

	if (bind(server_fd, (struct sockaddr *)&server_addr,
		 sizeof(server_addr)) == -1) {
		perror("Failed to bind socket");
		close(server_fd);
		return;
	}

	if (listen(server_fd, 1) == -1) {
		perror("Failed to listen on socket");
		close(server_fd);
		return;
	}

	printf("Server listening on port %d...\n", SERVER_PORT);

	client_fd =
		accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1) {
		perror("Failed to accept connection");
		close(server_fd);
		return;
	}

	printf("Client connected. Streaming video...\n");
    char *packet = (char *)malloc(sizeof(FrameHeader) + WIDTH*HEIGHT);
	char *buffer = (char *)malloc(sizeof(FrameHeader) + WIDTH*HEIGHT);
	while (1) {
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
		printf("Get frame width=%d,height=%d,size=%d\n",header.width,header.height,header.size);
		buf.bytesused = buf.m.planes[0].bytesused;
		header.size = buf.m.planes[0].bytesused;
		memcpy(packet, &header, sizeof(FrameHeader));
		memcpy(packet + sizeof(FrameHeader), buffers[buf.index].start, buf.bytesused);
        if (send(client_fd, packet, sizeof(FrameHeader) + buf.bytesused, 0) < 0) {
            perror("send packet");
            break;
        }
        
		if (recv(client_fd, buffer, WIDTH*HEIGHT, 0) < 0) {
        	perror("recv ack");
        	break;
        }
#if 0			
		// Send video frame data
		unsigned frame_info[4] = { fmt.fmt.pix.width,
					   fmt.fmt.pix.height,
					   fmt.fmt.pix.pixelformat,
					   buf.bytesused };
		if (send(client_fd, frame_info, sizeof(frame_info), 0) == -1) {
			perror("Failed to send frame info");
			break;
		}
		if (send(client_fd, buffers[buf.index].start, buf.bytesused,
			 0) == -1) {
			perror("Failed to send frame data");
			break;
		}
#endif
		if (ioctl(dev->fd, VIDIOC_QBUF, &buf) == -1) {
			perror("Failed to requeue buffer");
			break;
		}
	}

	free(packet);
	free(buffer);
	printf("Client disconnected.\n");

	close(client_fd);
	close(server_fd);

	if (ioctl(dev->fd, VIDIOC_STREAMOFF, &type) == -1) {
		perror("Failed to stop streaming");
	}

	for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i) {
		munmap(buffers[i].start, buffers[i].length);
	}
}

int main(int argc, char *argv[])
{
	static struct option long_options[] = {
		{ "device", required_argument, 0, 'd' },
		{ "cmd", required_argument, 0, 'c' },
		{ 0, 0, 0, 0 }
	};

	char *device = NULL;
	char *cmd = "info";
	struct device dev;
	int opt;

	while ((opt = getopt_long(argc, argv, "d:c:h?", long_options, NULL)) != -1) {
		switch (opt) {
		case 'd':
			device = optarg;
			break;
		case 'c':
			cmd = optarg;
			break;
		case 'h':
		case '?':
			break;
		default:
			print_usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (!device) {
		fprintf(stderr, "Error: --device is required.\n");
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}
	dev.fd = open(device, O_RDWR);
	if (dev.fd < 0) {
		perror("open: ");
		return EXIT_FAILURE;
	}

	if (!is_video_device(&dev)) {
		fprintf(stderr, "Error: %s is not a valid video device.\n",
			device);
		close(dev.fd);
		return EXIT_FAILURE;
	}

	if (!cmd) {
		fprintf(stderr, "Error: --cmd is required.\n");
		print_usage(argv[0]);
		close(dev.fd);
		return EXIT_FAILURE;
	}

	if (strcmp(cmd, "info") == 0) {
		print_device_info(&dev);
	} else if (strcmp(cmd, "stream") == 0) {
		stream_video(&dev);
	} else if (strcmp(cmd, "net_stream") == 0) {
		net_stream_video(&dev);
	} else {
		fprintf(stderr,
			"Error: Invalid command '%s'. Use 'info' or 'stream'.\n",
			cmd);
		print_usage(argv[0]);
		close(dev.fd);
		return EXIT_FAILURE;
	}

	close(dev.fd);
	return EXIT_SUCCESS;
}
