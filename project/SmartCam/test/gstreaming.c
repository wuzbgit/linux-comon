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
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/videodev2.h>

#define WIDTH 1920  // 设定视频宽度
#define HEIGHT 1080 // 设定视频高度
#define FRAME_SIZE (WIDTH * HEIGHT) // GRAY8 每个像素 1 字节
#define DEVICE "/dev/video0"

int start_stream(int fd)
{
    enum v4l2_buf_type buf_type;

	buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	if (ioctl(fd, VIDIOC_STREAMON, &buf_type) == -1) {
		perror("Failed to start streaming");
		return -1;
	}

    return 0;
}

static gboolean push_data(GstElement *appsrc) {
    static int fd = -1;
    static gboolean initialized = FALSE;
    GstBuffer *buffer;
    GstFlowReturn ret;
    guint size;
    GstMapInfo map;

    if (!initialized) {
        fd = open(DEVICE, O_RDWR);
        if (fd < 0) {
            g_printerr("Failed to open video device %s\n", DEVICE);
            return G_SOURCE_REMOVE;
        }

        start_stream(fd);

        initialized = TRUE;
    }

    // 分配 GStreamer 缓冲区
    size = FRAME_SIZE;
    buffer = gst_buffer_new_allocate(NULL, size, NULL);
    gst_buffer_map(buffer, &map, GST_MAP_WRITE);

    // 读取摄像头数据
    if (read(fd, map.data, size) < 0) {
        g_printerr("Failed to read from device\n");
        gst_buffer_unmap(buffer, &map);
        gst_buffer_unref(buffer);
        return G_SOURCE_REMOVE;
    }

    gst_buffer_unmap(buffer, &map);

    // 推送数据到 appsrc
    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
    gst_buffer_unref(buffer);

    if (ret != GST_FLOW_OK) {
        g_printerr("Error pushing buffer to appsrc\n");
        return G_SOURCE_REMOVE;
    }

    return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *appsrc, *convert, *encoder, *sink;
    GstCaps *caps;
    GstBus *bus;
    GstMessage *msg;
    GMainLoop *loop;

    gst_init(&argc, &argv);

    // 创建 GStreamer 元素
    pipeline = gst_pipeline_new("video-pipeline");
    appsrc = gst_element_factory_make("appsrc", "source");
    convert = gst_element_factory_make("videoconvert", "convert");
    encoder = gst_element_factory_make("jpegenc", "encoder");
    sink = gst_element_factory_make("tcpserversink", "sink");

    if (!pipeline || !appsrc || !convert || !encoder || !sink) {
        g_printerr("Failed to create pipeline elements\n");
        return -1;
    }

    // 设置 appsrc 的格式
    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "GRAY8",
                               "width", G_TYPE_INT, WIDTH,
                               "height", G_TYPE_INT, HEIGHT,
                               "framerate", GST_TYPE_FRACTION, 30, 1,
                               NULL);
    g_object_set(G_OBJECT(appsrc), "caps", caps, "format", GST_FORMAT_TIME, NULL);
    gst_caps_unref(caps);

    // 设置 jpegenc 质量
    g_object_set(G_OBJECT(encoder), "quality", 50, NULL);

    // 设置 tcpserversink
    g_object_set(G_OBJECT(sink), "host", "0.0.0.0", "port", 8080, NULL);

    // 组装 pipeline
    gst_bin_add_many(GST_BIN(pipeline), appsrc, convert, encoder, sink, NULL);
    if (!gst_element_link_many(appsrc, convert, encoder, sink, NULL)) {
        g_printerr("Failed to link elements\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // 启动 GStreamer pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // 创建 GMainLoop 以定期推送数据
    loop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(33, (GSourceFunc)push_data, appsrc);  // 30 FPS (1000ms / 30 ≈ 33ms)

    g_main_loop_run(loop);

    // 清理资源
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
