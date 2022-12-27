#include "gstbuffer_to_sink.h"

#include <gst/gst.h>
#include <gst/base/base.h>

enum playing_state {
    STOPPED,
    PLAYING
};

typedef struct _SkywayGstBufferToSinkPrivate {
    enum playing_state playing_state;
    guint num_buffers;
    guint max_buffers;
    GstQueueArray *queue;
} SkywayGstBufferToSinkPrivate;

#define DEFAULT_PROP_MAX_BUFFERS 1

G_DEFINE_TYPE_WITH_PRIVATE(SkywayGstBufferToSink, skyway_gstbuffer_to_sink,
                           SKYWAY_TYPE_APP_SINK_PROXY)

static void skyway_gstbuffer_to_sink_class_init(SkywayGstBufferToSinkClass *klass);

static void skyway_gstbuffer_to_sink_init(SkywayGstBufferToSink *self);

static gboolean skyway_gstbuffer_to_sink_play(SkywayGstBufferToSink *self);

static void skyway_gstbuffer_to_sink_stop(SkywayGstBufferToSink *self);

static GstSample *skyway_gstbuffer_to_sink_pull_sample(SkywayGstBufferToSink *self);

static void skyway_gstbuffer_to_sink_dispose(GObject *object);

static void skyway_gstbuffer_to_sink_class_init(SkywayGstBufferToSinkClass *klass) {
    g_print("skyway_gstbuffer_to_sink_class_init()\n");

    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = skyway_gstbuffer_to_sink_dispose;

    klass->parent_class.play = skyway_gstbuffer_to_sink_play;
    klass->parent_class.stop = skyway_gstbuffer_to_sink_stop;
    klass->parent_class.pull_sample = (GstSample *(*)(
            SkywayAppSinkProxy *)) skyway_gstbuffer_to_sink_pull_sample;
}

static void skyway_gstbuffer_to_sink_init(SkywayGstBufferToSink *self) {
    g_print("skyway_gstbuffer_to_sink_init()\n");
    SkywayGstBufferToSinkPrivate *priv = skyway_gstbuffer_to_sink_get_instance_private(self);
    priv->playing_state = STOPPED;
    priv->max_buffers = DEFAULT_PROP_MAX_BUFFERS;
    priv->num_buffers = 0;
    priv->queue = gst_queue_array_new(16);
}

SkywayGstBufferToSink *skyway_gstbuffer_to_sink_new() {
    return g_object_new(SKYWAY_TYPE_GSTBUFFER_TO_SINK, NULL);
}

static gboolean skyway_gstbuffer_to_sink_play(SkywayGstBufferToSink *self) {
    SkywayGstBufferToSinkPrivate *priv = skyway_gstbuffer_to_sink_get_instance_private(self);
    priv->playing_state = PLAYING;
    return TRUE;
}

static void skyway_gstbuffer_to_sink_stop(SkywayGstBufferToSink *self) {
    SkywayGstBufferToSinkPrivate *priv = skyway_gstbuffer_to_sink_get_instance_private(self);
    priv->playing_state = STOPPED;
    gst_queue_array_clear(priv->queue);

    skyway_app_sink_proxy_emit_eos(SKYWAY_APP_SINK_PROXY(self));
}

GstFlowReturn skyway_gstbuffer_to_sink_push_sample(SkywayGstBufferToSink *self, GstSample *sample) {
    SkywayGstBufferToSinkPrivate *priv = skyway_gstbuffer_to_sink_get_instance_private(self);

    if (priv->playing_state == STOPPED) {
        return GST_FLOW_OK;
    }

    while (priv->max_buffers > 0 && priv->num_buffers >= priv->max_buffers) {
        g_print("Dropping oldest sample\n");
        gst_queue_array_pop_head(priv->queue);
        priv->num_buffers--;
    }

    gst_queue_array_push_tail(priv->queue, sample);
    priv->num_buffers++;

    return skyway_app_sink_proxy_emit_new_sample(SKYWAY_APP_SINK_PROXY(self));
}

static GstSample *skyway_gstbuffer_to_sink_pull_sample(SkywayGstBufferToSink *self) {
    SkywayGstBufferToSinkPrivate *priv = skyway_gstbuffer_to_sink_get_instance_private(self);

    if (priv->num_buffers == 0) {
        g_printerr("No sample to pull!\n");
        return NULL;
    }

    GstSample *sample = gst_queue_array_pop_head(priv->queue);
    priv->num_buffers--;
    return sample;
}

static void skyway_gstbuffer_to_sink_dispose(GObject *object) {
    g_print("skyway_gstbuffer_to_sink_dispose()\n");
    SkywayGstBufferToSinkPrivate *priv = skyway_gstbuffer_to_sink_get_instance_private(
            SKYWAY_GSTBUFFER_TO_SINK(object));
    gst_queue_array_free(priv->queue);

    G_OBJECT_CLASS (skyway_gstbuffer_to_sink_parent_class)->dispose(object);
}
