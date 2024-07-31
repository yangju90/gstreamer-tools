
#include <gst/gst.h>

typedef struct _CustomData {
    GstElement* pipeline, * source, * filter, * sink;
    GstBus* bus;
    GstMessage* msg;
    GstStateChangeReturn ret;
} CustomData;


int process1(int* argc, char** argv[]) {
    CustomData data;

    gst_init(argc, argv);
    data.source = gst_element_factory_make("videotestsrc", "source");
    // todo filter
    data.filter = gst_element_factory_make("vertigotv", "vertigotv");
    data.sink = gst_element_factory_make("autovideosink", "sink");

    data.pipeline = gst_pipeline_new("test-pipeline");
    if (!data.pipeline || !data.source || !data.filter || !data.sink) {
        g_printerr("元素组件没有全部创建完成！");
    }

    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.filter, data.sink, NULL);
    if (!gst_element_link(data.source, data.filter) || !gst_element_link(data.filter, data.sink)) {
        g_printerr("元素组件链接失败！");
        gst_object_unref(data.pipeline);
        return -1;
    }

    g_object_set(data.source, "pattern", 0, NULL);
    data.ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
    if (data.ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("管道错误，不能播放！");
        gst_object_unref(data.pipeline);
        return -1;
    }
    data.bus = gst_element_get_bus(data.pipeline);

    data.msg = gst_bus_timed_pop_filtered(data.bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    if (data.msg != NULL) {
        GError* err;
        gchar* debug_info;

        switch (GST_MESSAGE_TYPE(data.msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(data.msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n",
                GST_OBJECT_NAME(data.msg->src), err->message);
            g_printerr("Debugging information: %s\n",
                debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            /* We should not reach here because we only asked for ERRORs and EOS */
            g_printerr("Unexpected message received.\n");
            break;
        }
        gst_message_unref(data.msg);
    }
    gst_object_unref(data.bus);
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);

    return 0;
}
