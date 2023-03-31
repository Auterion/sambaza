package com.auterion.sambaza

class RtspSrcProxyImpl(port: Int = 0) :
    RtspProxyImpl(port) {
    override fun addStream(streamInfo: StreamInfo) {
        println("Adding rtspsrc stream to ${streamInfo.location} (serving on ${streamInfo.path})")
        JniApi.addRtspSrcStream(skywayServerHandle, streamInfo.location, streamInfo.path)
    }
}