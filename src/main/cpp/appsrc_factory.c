#include <gst/app/gstappsrc.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#include "appsrc_factory.h"

G_DEFINE_TYPE(AppRtspMedia, app_rtsp_media, GST_TYPE_RTSP_MEDIA)

G_DEFINE_TYPE(AppSrcFactory, app_src_factory, GST_TYPE_RTSP_MEDIA_FACTORY)

static gboolean (*default_prepare)(GstRTSPMedia *, GstRTSPThread *);

static gboolean (*default_unprepare)(GstRTSPMedia *);

static GstFlowReturn new_sample_handler(SkywayAppSinkProxy *sink, AppRtspMedia *media);

static void eos_handler(__attribute__ ((unused)) SkywayAppSinkProxy *src, GstAppSrc *appsrc);

static gboolean custom_media_prepare(GstRTSPMedia *media, GstRTSPThread *thread);

static gboolean custom_media_unprepare(GstRTSPMedia *media);

static void app_rtsp_media_init(__attribute__ ((unused)) AppRtspMedia *media);

static void app_rtsp_media_class_init(AppRtspMediaClass *klass);

static GstRTSPMedia *app_src_factory_construct(GstRTSPMediaFactory *factory, const GstRTSPUrl *url);

static GstElement *extract_element_by_name(GstRTSPMedia *media, const gchar *name);

static GstElement *extract_element_by_name(GstRTSPMedia *media, const gchar *name) {
    GstElement *element = gst_rtsp_media_get_element(media);
    if (!element) {
        g_printerr("No element in media\n");
        return NULL;
    }

    GstIterator *it = gst_bin_iterate_sources(GST_BIN(element));
    if (!it) {
        g_printerr("Element does not seem to be a valid iterator...\n");
        return NULL;
    }

    GstElement *candidate_src;
    gboolean done = FALSE;

    while (!done) {
        GValue value = G_VALUE_INIT;
        switch (gst_iterator_next(it, &value)) {
            case GST_ITERATOR_OK: {
                candidate_src = GST_ELEMENT(g_value_get_object(&value));
                gchar *candidate_name = gst_element_get_name(candidate_src);
                if (candidate_name) {
                    if (strstr(candidate_name, name) != NULL) {
                        done = TRUE;
                    }
                }
                g_value_unset(&value);
                break;
            }
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            case GST_ITERATOR_ERROR:
            case GST_ITERATOR_DONE:
            default:
                done = TRUE;
                break;
        }
    }

    gst_iterator_free(it);
    g_object_unref(element);
    return candidate_src;
}

static GstFlowReturn new_sample_handler(SkywayAppSinkProxy *sink, AppRtspMedia *media) {
    GstSample *sample;
    g_signal_emit_by_name(sink, "pull-sample", &sample);

    if (sample) {
        if (!GST_IS_RTSP_MEDIA(&media->parent)) {
            g_printerr("Media invalid, disconnecting signal\n");
            g_signal_handler_disconnect(media->appsink, media->new_sample_handle);
            return GST_FLOW_ERROR;
        }

        GstAppSrc *appsrc = GST_APP_SRC(extract_element_by_name(&media->parent, "appsrc"));
        if (!appsrc) {
            return GST_FLOW_ERROR;
        }

        gst_app_src_push_sample(appsrc, sample);
        gst_sample_unref(sample);

        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}

static void eos_handler(__attribute__ ((unused)) SkywayAppSinkProxy *src, GstAppSrc *appsrc) {
    gst_element_send_event(GST_ELEMENT(appsrc), gst_event_new_eos());
}

static gboolean custom_media_prepare(GstRTSPMedia *media, GstRTSPThread *thread) {
    if (!default_prepare(media, thread)) {
        g_printerr("Default prepare() failed!\n");
        return FALSE;
    }

    AppRtspMedia *self = APP_RTSP_MEDIA(media);

    if (!skyway_app_sink_proxy_play(self->appsink)) {
        custom_media_unprepare(media);
        return FALSE;
    }

    self->new_sample_handle = g_signal_connect(self->appsink, "new-sample",
                                               G_CALLBACK(new_sample_handler), self);

    GstAppSrc *app_src = GST_APP_SRC(extract_element_by_name(media, "appsrc"));
    if (!app_src) {
        g_printerr("No appsrc found in media!\n");
    }

    gst_app_src_set_leaky_type(app_src, GST_APP_LEAKY_TYPE_NONE);
    g_object_set(app_src, "max-buffers", 5, NULL);

    self->eos_handle = g_signal_connect(self->appsink, "eos", G_CALLBACK(eos_handler), app_src);
    return TRUE;
}

static gboolean custom_media_unprepare(GstRTSPMedia *media) {
    AppRtspMedia *self = APP_RTSP_MEDIA(media);

    skyway_app_sink_proxy_stop(self->appsink);
    g_signal_handler_disconnect(self->appsink, self->new_sample_handle);
    g_signal_handler_disconnect(self->appsink, self->eos_handle);

    return default_unprepare(media);
}

static void app_rtsp_media_init(__attribute__ ((unused)) AppRtspMedia *media) {}

static void app_rtsp_media_class_init(AppRtspMediaClass *klass) {
    GstRTSPMediaClass *parent_klass = GST_RTSP_MEDIA_CLASS(klass);
    if (!default_prepare) {
        default_prepare = parent_klass->prepare;
    }
    parent_klass->prepare = custom_media_prepare;

    if (!default_unprepare) {
        default_unprepare = parent_klass->unprepare;
    }
    parent_klass->unprepare = custom_media_unprepare;
}

static void app_src_factory_class_init(AppSrcFactoryClass *klass) {
    GstRTSPMediaFactoryClass *mf_class = GST_RTSP_MEDIA_FACTORY_CLASS(klass);
    mf_class->construct = app_src_factory_construct;
}

static void app_src_factory_init(__attribute__ ((unused)) AppSrcFactory *factory) {}

AppSrcFactory *app_src_factory_new() {
    return g_object_new(app_src_factory_get_type(), NULL);
}

static GstRTSPMedia *
app_src_factory_construct(GstRTSPMediaFactory *factory, const GstRTSPUrl *url) {
    GstRTSPMediaFactoryClass *klass = GST_RTSP_MEDIA_FACTORY_GET_CLASS(factory);
    if (!klass->create_pipeline) {
        g_printerr("No create_pipeline function!");
        return NULL;
    }

    GstElement *element = gst_rtsp_media_factory_create_element(factory, url);
    if (!element) {
        g_printerr("Could not create element!");
        return NULL;
    }

    AppRtspMedia *media = g_object_new(app_rtsp_media_get_type(), "element", element, NULL);
    media->appsink = APP_SRC_FACTORY(factory)->appsink;

    gst_rtsp_media_collect_streams(GST_RTSP_MEDIA(media));

    GstElement *pipeline = klass->create_pipeline(factory, GST_RTSP_MEDIA(media));
    if (!pipeline) {
        g_printerr("Could not create pipeline!");
        g_object_unref(media);
        return NULL;
    }

    gst_rtsp_media_set_reusable(GST_RTSP_MEDIA(media), FALSE);
    return GST_RTSP_MEDIA(media);
}
