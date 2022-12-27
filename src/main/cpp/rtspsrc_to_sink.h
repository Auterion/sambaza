#ifndef SKYWAY_RTSPSRC_TO_SINK_H
#define SKYWAY_RTSPSRC_TO_SINK_H

#include "appsink_proxy.h"

G_BEGIN_DECLS

#define SKYWAY_TYPE_RTSP_SRC_TO_SINK (skyway_rtsp_src_to_sink_get_type())

typedef struct _SkywayRtspSrcToSink {
    SkywayAppSinkProxy parent;
} SkywayRtspSrcToSink;

G_DECLARE_FINAL_TYPE(SkywayRtspSrcToSink, skyway_rtsp_src_to_sink, SKYWAY, RTSP_SRC_TO_SINK,
                     SkywayAppSinkProxy)

GType skyway_rtsp_src_to_sink_get_type(void);

SkywayRtspSrcToSink *skyway_rtsp_src_to_sink_new();

gboolean skyway_rtsp_src_to_sink_prepare(SkywayRtspSrcToSink *self, const char *location);

G_END_DECLS

#endif // SKYWAY_RTSPSRC_TO_SINK_H
