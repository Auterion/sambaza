package com.auterion.sambaza

interface PushableProxy : RtspProxy {
    fun pushFrame(frame: H264Frame)
}