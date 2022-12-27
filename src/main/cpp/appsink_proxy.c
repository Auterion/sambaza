#include <gst/gst.h>

#include "appsink_proxy.h"

typedef struct _SkywayAppSinkProxyPrivate {
    GstElement *pipeline;
} SkywayAppSinkProxyPrivate;

enum {
    // signals
    SIGNAL_EOS,
    SIGNAL_NEW_SAMPLE,
    SIGNAL_START_PLAYING,
    SIGNAL_STOP_PLAYING,

    // actions
    SIGNAL_PULL_SAMPLE,

    LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE(SkywayAppSinkProxy, skyway_app_sink_proxy, G_TYPE_OBJECT)

static void skyway_app_sink_proxy_class_init(SkywayAppSinkProxyClass *klass);

static void skyway_app_sink_proxy_init(__attribute__ ((unused)) SkywayAppSinkProxy *self);

static gboolean default_play(SkywayAppSinkProxy *self);

static void default_stop(SkywayAppSinkProxy *self);

static guint skyway_app_sink_proxy_signals[LAST_SIGNAL] = {0};

static void skyway_app_sink_proxy_class_init(SkywayAppSinkProxyClass *klass) {
    g_print("skyway_app_sink_proxy_class_init()\n");

    skyway_app_sink_proxy_signals[SIGNAL_EOS] =
            g_signal_new("eos", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                         G_STRUCT_OFFSET(SkywayAppSinkProxyClass, eos),
                         NULL, NULL, NULL, G_TYPE_NONE, 0, G_TYPE_NONE);

    skyway_app_sink_proxy_signals[SIGNAL_NEW_SAMPLE] =
            g_signal_new("new-sample", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                         G_STRUCT_OFFSET(SkywayAppSinkProxyClass, new_sample),
                         NULL, NULL, NULL, GST_TYPE_FLOW_RETURN, 0, G_TYPE_NONE);

    skyway_app_sink_proxy_signals[SIGNAL_START_PLAYING] =
            g_signal_new("start-playing", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                         G_STRUCT_OFFSET(SkywayAppSinkProxyClass, start_playing),
                         NULL, NULL, NULL, G_TYPE_NONE, 0, G_TYPE_NONE);

    skyway_app_sink_proxy_signals[SIGNAL_STOP_PLAYING] =
            g_signal_new("stop-playing", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                         G_STRUCT_OFFSET(SkywayAppSinkProxyClass, stop_playing),
                         NULL, NULL, NULL, G_TYPE_NONE, 0, G_TYPE_NONE);

    skyway_app_sink_proxy_signals[SIGNAL_PULL_SAMPLE] =
            g_signal_new("pull-sample", G_TYPE_FROM_CLASS(klass),
                         G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                         G_STRUCT_OFFSET(SkywayAppSinkProxyClass, pull_sample),
                         NULL, NULL, NULL, GST_TYPE_SAMPLE, 0, G_TYPE_NONE);

    klass->play = default_play;
    klass->stop = default_stop;
}

static void skyway_app_sink_proxy_init(__attribute__ ((unused)) SkywayAppSinkProxy *self) {
    g_print("skyway_app_sink_proxy_init()\n");
}

SkywayAppSinkProxy *skyway_app_sink_proxy_new() {
    return g_object_new(SKYWAY_TYPE_APP_SINK_PROXY, NULL);
}

gboolean skyway_app_sink_proxy_play(SkywayAppSinkProxy *self) {
    SkywayAppSinkProxyClass *klass = SKYWAY_APP_SINK_PROXY_GET_CLASS(self);
    if (!klass->play(self)) {
        return FALSE;
    }

    g_signal_emit(self, skyway_app_sink_proxy_signals[SIGNAL_START_PLAYING], 0);
    return TRUE;
}

void skyway_app_sink_proxy_stop(SkywayAppSinkProxy *self) {
    SkywayAppSinkProxyClass *klass = SKYWAY_APP_SINK_PROXY_GET_CLASS(self);
    klass->stop(self);
    g_signal_emit(self, skyway_app_sink_proxy_signals[SIGNAL_STOP_PLAYING], 0);
}

static gboolean default_play(__attribute__ ((unused)) SkywayAppSinkProxy *self) {
    return TRUE;
}

static void default_stop(__attribute__ ((unused)) SkywayAppSinkProxy *self) {}

GstFlowReturn skyway_app_sink_proxy_emit_new_sample(SkywayAppSinkProxy *self) {
    GstFlowReturn ret;
    g_signal_emit(self, skyway_app_sink_proxy_signals[SIGNAL_NEW_SAMPLE], 0, &ret);
    return ret;
}

void skyway_app_sink_proxy_emit_eos(SkywayAppSinkProxy *self) {
    g_signal_emit(self, skyway_app_sink_proxy_signals[SIGNAL_EOS], 0);
}
