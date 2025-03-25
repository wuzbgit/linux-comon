#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *capsfilter, *convert, *encoder, *sink;
    GstCaps *caps;
    GstBus *bus;
    GstMessage *msg;
    GError *error = NULL;

    // 初始化 GStreamer
    gst_init(&argc, &argv);

    // 创建元素
    pipeline = gst_pipeline_new("video-pipeline");
    source = gst_element_factory_make("v4l2src", "source");
    capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    convert = gst_element_factory_make("videoconvert", "convert");
    encoder = gst_element_factory_make("jpegenc", "encoder");
    sink = gst_element_factory_make("tcpserversink", "sink");

    if (!pipeline || !source || !capsfilter || !convert || !encoder || !sink) {
        g_printerr("Failed to create elements.\n");
        return -1;
    }

    // 设置 v4l2src 设备
    g_object_set(G_OBJECT(source), "device", "/dev/video0", NULL);

    // 设置 capsfilter 以匹配 GRAY8 格式
    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "GRAY8",
                               NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    gst_caps_unref(caps);

    // 设置 jpegenc 质量
    g_object_set(G_OBJECT(encoder), "quality", 50, NULL);

    // 设置 tcpserversink
    g_object_set(G_OBJECT(sink), "host", "0.0.0.0", "port", 8080, NULL);

    // 构建管道
    gst_bin_add_many(GST_BIN(pipeline), source, capsfilter, convert, encoder, sink, NULL);
    if (!gst_element_link_many(source, capsfilter, convert, encoder, sink, NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // 运行管道
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // 监听总线消息，等待 EOS 或错误
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // 处理消息
    if (msg != NULL) {
        gst_message_parse_error(msg, &error, NULL);
        g_printerr("Error: %s\n", error->message);
        g_clear_error(&error);
        gst_message_unref(msg);
    }

    // 清理资源
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
