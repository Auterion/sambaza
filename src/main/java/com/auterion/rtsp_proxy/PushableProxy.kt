package com.auterion.rtsp_proxy

interface PushableProxy : RtspProxy {
    fun pushFrame(frame: H264Frame)
}