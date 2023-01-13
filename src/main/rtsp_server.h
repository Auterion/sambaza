#ifndef SKYWAY_RTSP_SERVER_H
#define SKYWAY_RTSP_SERVER_H

typedef struct _SkywayRtspServer {
    GstRTSPServer *server;
    void *src;
    int port;
} SkywayRtspServer;

SkywayRtspServer *skyway_rtsp_server_new();

int skyway_add_rtspsrc_stream(SkywayRtspServer *server, const char *location, const char *path);

void skyway_add_pushable_stream(SkywayRtspServer *server, const char *path);

void skyway_remove_stream(SkywayRtspServer *server, const char *path);

#endif //SKYWAY_RTSP_SERVER_H
