#ifndef SKYWAY_GSTBUFFER_TO_SINK_H
#define SKYWAY_GSTBUFFER_TO_SINK_H

#include "appsink_proxy.h"

G_BEGIN_DECLS

#define SKYWAY_TYPE_GSTBUFFER_TO_SINK (skyway_gstbuffer_to_sink_get_type())

typedef struct _SkywayGstBufferToSink {
    SkywayAppSinkProxy parent;
} SkywayGstBufferToSink;

G_DECLARE_FINAL_TYPE(SkywayGstBufferToSink, skyway_gstbuffer_to_sink, SKYWAY, GSTBUFFER_TO_SINK, SkywayAppSinkProxy)
GType skyway_gstbuffer_to_sink_get_type(void);

SkywayGstBufferToSink* skyway_gstbuffer_to_sink_new();
GstFlowReturn skyway_gstbuffer_to_sink_push_sample(SkywayGstBufferToSink* self, GstSample* sample);

G_END_DECLS

#endif // SKYWAY_GSTBUFFER_TO_SINK_H
