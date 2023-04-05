#include <jni.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#include "appsink_proxy.h"
#include "gstbuffer_to_sink.h"
#include "rtsp_server.h"

GST_PLUGIN_STATIC_DECLARE(app);

GST_PLUGIN_STATIC_DECLARE(coreelements);

GST_PLUGIN_STATIC_DECLARE(rtp);

GST_PLUGIN_STATIC_DECLARE(rtpmanager);

GST_PLUGIN_STATIC_DECLARE(rtsp);

GST_PLUGIN_STATIC_DECLARE(tcp);

GST_PLUGIN_STATIC_DECLARE(udp);

GST_PLUGIN_STATIC_DECLARE(videoparsersbad);


typedef struct _SkywayHandles {
    GMainLoop *main_loop;
    guint server_handle; // TODO have a collection of handles for each server -> actually we want one server but multiple streams
} SkywayHandles;

JNIEXPORT jlong JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_initNative(__attribute__ ((unused)) JNIEnv *env,
                                                               __attribute__ ((unused)) jobject thiz) {
    gst_debug_set_default_threshold(GST_LEVEL_NONE);

    GError *err;
    gboolean init_succeeded = gst_init_check(NULL, NULL, &err);
    if (!init_succeeded) {
        g_printerr("Error initializing gstreamer: %s\n", err->message);
        return 0;
    }

    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(rtp);
    GST_PLUGIN_STATIC_REGISTER(rtpmanager);
    GST_PLUGIN_STATIC_REGISTER(rtsp);
    GST_PLUGIN_STATIC_REGISTER(tcp);
    GST_PLUGIN_STATIC_REGISTER(udp);
    GST_PLUGIN_STATIC_REGISTER(videoparsersbad);

    SkywayHandles *handles = malloc(sizeof(SkywayHandles));
    handles->main_loop = g_main_loop_new(NULL, FALSE);

    return (jlong) handles;
}

JNIEXPORT void JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_runMainLoopNative(
        __attribute__ ((unused)) JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jlong main_loop_handle) {
    SkywayHandles *handles = (SkywayHandles *) main_loop_handle;

    if (handles->main_loop && !g_main_loop_is_running(handles->main_loop)) {
        g_main_loop_run(handles->main_loop);
    }
}

JNIEXPORT jlong JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_createRtspServerNative(
        __attribute__ ((unused)) JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jint port) {
    jlong server = (jlong) skyway_rtsp_server_new(port);
    return server;
}

JNIEXPORT jint JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_getPortNative(
        __attribute__ ((unused)) JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jlong skyway_server_handle) {
    SkywayRtspServer *server = (SkywayRtspServer *) skyway_server_handle;
    return server->port;
}

JNIEXPORT void JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_startNative(
        __attribute__ ((unused)) JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jlong skyway_server_handle,
        jlong main_loop_handle) {
    SkywayHandles *handles = (SkywayHandles *) main_loop_handle;

    SkywayRtspServer *server = (SkywayRtspServer *) skyway_server_handle;
    handles->server_handle = (jlong) gst_rtsp_server_attach(server->server, NULL);
    server->port = gst_rtsp_server_get_bound_port(server->server);
}

JNIEXPORT void JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_stopNative(__attribute__ ((unused)) JNIEnv *env,
                                                               __attribute__ ((unused)) jobject thiz,
                                                               jlong skyway_server_handle,
                                                               jlong main_loop_handle) {
    SkywayHandles *handles = (SkywayHandles *) main_loop_handle;

    SkywayRtspServer *server = (SkywayRtspServer *) skyway_server_handle;
    // TODO server->src is set only for pushable proxy
    if (server->src) {
        skyway_app_sink_proxy_stop(server->src);
    }
    g_source_remove(handles->server_handle);
    g_object_unref(server->server);

    g_main_loop_quit(handles->main_loop);
}

JNIEXPORT void JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_addRtspSrcStreamNative(
        JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jlong skyway_server_handle,
        jstring location,
        jstring path) {

    const char *native_location = (*env)->GetStringUTFChars(env, location, 0);
    const char *native_path = (*env)->GetStringUTFChars(env, path, 0);

    SkywayRtspServer *server = (SkywayRtspServer *) skyway_server_handle;
    skyway_add_rtspsrc_stream(server, native_location, native_path);

    (*env)->ReleaseStringUTFChars(env, location, native_location);
    (*env)->ReleaseStringUTFChars(env, path, native_path);
}

JNIEXPORT void JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_addPushableStreamNative(
        JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jlong skyway_server_handle,
        jstring path) {

    const char *native_path = (*env)->GetStringUTFChars(env, path, 0);

    SkywayRtspServer *server = (SkywayRtspServer *) skyway_server_handle;
    skyway_add_pushable_stream(server, native_path);

    (*env)->ReleaseStringUTFChars(env, path, native_path);
}

JNIEXPORT void JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_removeStreamNative(
        JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jlong skyway_server_handle,
        jstring path) {
    const char *native_path = (*env)->GetStringUTFChars(env, path, 0);
    SkywayRtspServer *server = (SkywayRtspServer *) skyway_server_handle;
    skyway_remove_stream(server, native_path);
    (*env)->ReleaseStringUTFChars(env, path, native_path);
}

JNIEXPORT void JNICALL
Java_com_auterion_sambaza_JniApi_00024Companion_pushFrameNative(
        JNIEnv *env,
        __attribute__ ((unused)) jobject thiz,
        jlong skyway_server_handle,
        jlong pts,
        jbyteArray buffer,
        jstring caps) {
    jbyte *buffer_ptr = (*env)->GetByteArrayElements(env, buffer, NULL);
    jsize buffer_size = (*env)->GetArrayLength(env, buffer);
    const char *native_caps = (*env)->GetStringUTFChars(env, caps, 0);

    SkywayRtspServer *server = (SkywayRtspServer *) skyway_server_handle;
    SkywayGstBufferToSink *gst_buffer_to_sink = (SkywayGstBufferToSink *) server->src;

    GstBuffer *gst_buffer = gst_buffer_new_wrapped(buffer_ptr, buffer_size);

    if (pts == -1) {
        GST_BUFFER_PTS(gst_buffer) = GST_CLOCK_TIME_NONE;
    } else {
        GST_BUFFER_PTS(gst_buffer) = pts;
    }

    GstCaps *gst_caps = NULL;
    if (strlen(native_caps) > 0) {
        gst_caps = gst_caps_from_string(native_caps);
    }

    GstSample *sample = gst_sample_new(gst_buffer, gst_caps, NULL, NULL);
    skyway_gstbuffer_to_sink_push_sample(gst_buffer_to_sink, sample);
    gst_sample_unref(sample);

    (*env)->ReleaseByteArrayElements(env, buffer, buffer_ptr, JNI_ABORT);
    (*env)->ReleaseStringUTFChars(env, caps, native_caps);
}
