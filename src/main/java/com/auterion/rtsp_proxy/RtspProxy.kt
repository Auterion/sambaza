package com.auterion.rtsp_proxy

interface RtspProxy {
    fun start()
    fun stop()
    fun getPort(): Int
}