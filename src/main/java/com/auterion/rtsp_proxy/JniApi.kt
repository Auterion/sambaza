package com.auterion.rtsp_proxy

internal class JniApi {
    companion object {
        private val handles: Long

        init {
            handles = initNative()

            if (handles == 0.toLong()) {
                throw RuntimeException("Gstreamer initialization failed!")
            }
        }

        private external fun initNative(): Long

        internal fun runMainLoop() {
            runMainLoopNative(handles)
        }

        private external fun runMainLoopNative(mainLoopHandle: Long)

        internal fun createRtspServer(port: Int = 0): Long {
            val server = createRtspServerNative(port)

            if (server == 0L) {
                throw RuntimeException("Failed to create server")
            }

            return server;
        }

        private external fun createRtspServerNative(port: Int): Long

        internal fun getPort(serverHandle: Long): Int {
            return getPortNative(serverHandle)
        }

        private external fun getPortNative(skywayServerHandle: Long): Int

        internal fun start(serverHandle: Long) {
            startNative(serverHandle, handles)
        }

        private external fun startNative(skywayServerHandle: Long, mainLoopHandle: Long)

        internal fun stop(serverHandle: Long) {
            stopNative(serverHandle, handles)
        }

        private external fun stopNative(skywayServerHandle: Long, mainLoopHandle: Long)

        internal fun addRtspSrcStream(serverHandle: Long, location: String, path: String) {
            addRtspSrcStreamNative(serverHandle, location, path)
        }

        private external fun addRtspSrcStreamNative(
            skywayServerHandle: Long,
            location: String,
            path: String
        )

        internal fun addPushableStream(serverHandle: Long, path: String) {
            addPushableStreamNative(serverHandle, path)
        }

        private external fun addPushableStreamNative(skywayServerHandle: Long, path: String)

        internal fun removeStream(serverHandle: Long, path: String) {
            removeStreamNative(serverHandle, path)
        }

        private external fun removeStreamNative(skywayServerHandle: Long, path: String)

        internal fun pushFrame(serverHandle: Long, frame: H264Frame) {
            val pts = if (frame.pts == ULong.MAX_VALUE) -1 else frame.pts.toLong()
            pushFrameNative(
                serverHandle,
                pts,
                frame.buffer(),
                frame.caps ?: ""
            )
        }

        private external fun pushFrameNative(
            skywayServerHandle: Long,
            pts: Long,
            buffer: ByteArray,
            caps: String
        )
    }
}