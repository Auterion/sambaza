package com.auterion.sambaza

class PushableProxyImpl(port: Int = 0) :
    RtspProxyImpl(port), PushableProxy {
    private var wasStreamAdded = false

    override fun addStream(streamInfo: StreamInfo) {
        if (wasStreamAdded) {
            throw RuntimeException("PushableProxy currently only supports exactly one stream!")
        }

        println("Adding pushable stream (serving on ${streamInfo.path})")
        JniApi.addPushableStream(skywayServerHandle, streamInfo.path)
        wasStreamAdded = true
    }

    override fun pushFrame(frame: H264Frame) {
        if (!wasStreamAdded) return
        JniApi.pushFrame(skywayServerHandle, frame)
    }
}
