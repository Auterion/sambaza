#ifndef SKYWAY_APPSINK_PROXY_H
#define SKYWAY_APPSINK_PROXY_H

#include <glib.h>
#include <glib-object.h>
#include <gst/gst.h>

G_BEGIN_DECLS

#define SKYWAY_TYPE_APP_SINK_PROXY (skyway_app_sink_proxy_get_type())

G_DECLARE_DERIVABLE_TYPE(SkywayAppSinkProxy, skyway_app_sink_proxy, SKYWAY, APP_SINK_PROXY, GObject)

GType skyway_app_sink_proxy_get_type(void);

typedef struct _SkywayAppSinkProxyClass {
    GObjectClass parent;

    // signals
    void (*eos)(SkywayAppSinkProxy *skyway_app_sink_proxy);

    GstFlowReturn (*new_sample)(SkywayAppSinkProxy *skyway_app_sink_proxy);

    void (*start_playing)(SkywayAppSinkProxy *skyway_app_sink_proxy);

    void (*stop_playing)(SkywayAppSinkProxy *skyway_app_sink_proxy);

    // actions
    GstSample *(*pull_sample)(SkywayAppSinkProxy *skyway_app_sink_proxy);

    gboolean (*play)();

    void (*stop)();
} SkywayAppSinkProxyClass;


SkywayAppSinkProxy *skyway_app_sink_proxy_new();

gboolean skyway_app_sink_proxy_play(SkywayAppSinkProxy *self);

void skyway_app_sink_proxy_stop(SkywayAppSinkProxy *self);

GstFlowReturn skyway_app_sink_proxy_emit_new_sample(SkywayAppSinkProxy *self);

void skyway_app_sink_proxy_emit_eos(SkywayAppSinkProxy *self);

G_END_DECLS

#endif // SKYWAY_APPSINK_PROXY_H
