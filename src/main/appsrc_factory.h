#ifndef SKYWAY_APPSRC_FACTORY_H
#define SKYWAY_APPSRC_FACTORY_H

#include "appsink_proxy.h"

G_BEGIN_DECLS

struct _AppRtspMedia {
    GstRTSPMedia parent;
    SkywayAppSinkProxy *appsink;
    gulong new_sample_handle;
    gulong eos_handle;
};

struct _AppSrcFactory {
    GstRTSPMediaFactory parent;
    SkywayAppSinkProxy *appsink;
};

G_DECLARE_FINAL_TYPE(AppRtspMedia, app_rtsp_media, APP_RTSP, MEDIA, GstRTSPMedia)

G_DECLARE_FINAL_TYPE(AppSrcFactory, app_src_factory, APP_SRC, FACTORY, GstRTSPMediaFactory)

AppSrcFactory *app_src_factory_new();

void app_src_push_buffer(AppSrcFactory *app_src_factory, GstBuffer *buffer);

G_END_DECLS

#endif // SKYWAY_APPSRC_FACTORY_H
