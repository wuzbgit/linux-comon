#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "ringbuffer.h"

#define SOCKET_PATH "/tmp/video_control.sock"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct gstreamer_handle
{
    GstElement *pipeline;
    GstElement *appsrc;
    int start;
    int init;
};

static struct gstreamer_handle g_handle;

static void buffer_release_cb(gpointer data) {
    frame_item *frame = (frame_item *)data;
    printf("realse frame:width=%d,height=%d\n",frame->width,frame->height);
    ringbuffer_release(frame);
}

static gboolean need_data(GstElement *appsrc, guint unused, gpointer user_data)
{
    GstBuffer *gst_buffer;
    GstFlowReturn ret;
    frame_item *frame;
    static guint64 timestamp = 0;

    if (!(frame=ringbuffer_pop())) {
        printf("gstreamer can not get frame from ringbuffer\n");
        return G_SOURCE_CONTINUE;
    }

    gst_buffer = gst_buffer_new_allocate(NULL, frame->size, NULL);
    // GstMemory *memory;
    // memory = gst_memory_new_wrapped(0, frame->buf, frame->size, 0, frame->size, frame, buffer_release_cb);
    // gst_buffer_append_memory(gst_buffer, memory);

    gst_buffer_fill(gst_buffer, 0, frame->buf, frame->size);

    GST_BUFFER_PTS(gst_buffer) = timestamp;
    GST_BUFFER_DURATION(gst_buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 30);
    timestamp += GST_BUFFER_DURATION(gst_buffer);

    g_signal_emit_by_name(appsrc, "push-buffer", gst_buffer, &ret);

    gst_buffer_unref(gst_buffer);

    ringbuffer_release(frame);
    return G_SOURCE_CONTINUE;
}

int gstreamer_start_pipe()
{
    if (g_handle.init == 0) {
        return -1;
    }

    pthread_mutex_lock(&lock);
    gst_element_set_state(g_handle.pipeline, GST_STATE_PLAYING);
    pthread_mutex_unlock(&lock);

    g_handle.start = 1;

    return 0;
}

int gstreamer_stop_pipe()
{
    pthread_mutex_lock(&lock);
    gst_element_set_state(g_handle.pipeline, GST_STATE_PAUSED);
    pthread_mutex_unlock(&lock);

    return 0;
}

void gstreamer_set_pipeline_resolution(int width, int height)
{
    pthread_mutex_lock(&lock);
    if (g_handle.appsrc) {
        gst_element_set_state(g_handle.pipeline, GST_STATE_PAUSED);

        GstCaps *new_caps = gst_caps_new_simple("video/x-raw",
                                            "format", G_TYPE_STRING, "GRAY8",
                                            "width", G_TYPE_INT, width,
                                            "height", G_TYPE_INT, height,
                                            "framerate", GST_TYPE_FRACTION, 30, 1,
                                            NULL);
        g_object_set(G_OBJECT(g_handle.appsrc), "caps", new_caps, NULL);
        gst_caps_unref(new_caps);

        gst_element_set_state(g_handle.pipeline, GST_STATE_PLAYING);
    }

    pthread_mutex_unlock(&lock);
}

int gstreamer_create_pipeline(unsigned width, unsigned height)
{
    GstElement *convert, *encoder, *sink;
    GstCaps *caps;
    pthread_t cmd_thread;

    gst_init(NULL, NULL);

    g_handle.pipeline = gst_pipeline_new("video-stream-pipeline");
    g_handle.appsrc = gst_element_factory_make("appsrc", "source");
    convert = gst_element_factory_make("videoconvert", "convert");
#ifdef JPEG_HARD
    encoder = gst_element_factory_make("mppjpegenc", "encoder");
#else
    encoder = gst_element_factory_make("jpegenc", "encoder");
#endif
    sink = gst_element_factory_make("tcpserversink", "sink");

    if (!g_handle.pipeline || !g_handle.appsrc || !convert || !encoder || !sink) {
        g_printerr("Failed to create elements.\n");
        return -1;
    }

    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "GRAY8",
                               "width", G_TYPE_INT, width,
                               "height", G_TYPE_INT, height,
                               "framerate", GST_TYPE_FRACTION, 30, 1,
                               NULL);
    g_object_set(G_OBJECT(g_handle.appsrc), "caps", caps, "format", GST_FORMAT_TIME, NULL);
    gst_caps_unref(caps);
#ifdef JPEG_HARD
    g_object_set(G_OBJECT(encoder), "q-factor", 50, NULL);
#else
    g_object_set(G_OBJECT(encoder), "quality", 50, NULL);
#endif
    g_object_set(G_OBJECT(sink), "host", "0.0.0.0", "port", 8080, NULL);

    gst_bin_add_many(GST_BIN(g_handle.pipeline), g_handle.appsrc, convert, encoder, sink, NULL);
    if (!gst_element_link_many(g_handle.appsrc, convert, encoder, sink, NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(g_handle.pipeline);
        return -1;
    }

    g_signal_connect(g_handle.appsrc, "need-data", G_CALLBACK(need_data), NULL);

    g_handle.init = 1;

    return 0;
}

void gstream_loop()
{
    if (g_handle.init) {
        GMainLoop *loop;

    GstStateChangeReturn ret = gst_element_set_state(g_handle.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to start pipeline.\n");
        gst_object_unref(g_handle.pipeline);
        return ;
    }

        g_print("Streaming started at tcp://0.0.0.0:8080\n");
        loop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run(loop);

        g_main_loop_unref(loop);
        gst_element_set_state(g_handle.pipeline, GST_STATE_NULL);
        gst_object_unref(g_handle.pipeline);
    }
}
