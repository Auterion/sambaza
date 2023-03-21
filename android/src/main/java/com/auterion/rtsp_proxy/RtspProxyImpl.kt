package com.auterion.rtsp_proxy

import com.auterion.rtsp_proxy.StreamInfo.Companion.extractStreamInfo
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.launch
import java.util.*
import kotlin.coroutines.CoroutineContext

abstract class RtspProxyImpl(port: Int = 0) : RtspProxy, CoroutineScope {
    override val coroutineContext: CoroutineContext = Job() + Dispatchers.IO
    private var videoStreamFlowCollectJob: Job? = null

    init {
        println("Loading libsambaza.so")
        System.loadLibrary("sambaza")
    }

    protected val skywayServerHandle: Long = JniApi.createRtspServer(port)
    private val streams: MutableMap<Int, StreamInfo> = Collections.synchronizedMap(HashMap())

    init {
        launch {
            println("Running glib main loop")
            JniApi.runMainLoop()
        }
    }

    fun setVideoStreamFlow(videoStreamFlow: Flow<Triple<Int, String, String>>) {
        videoStreamFlowCollectJob = launch {
            videoStreamFlow.collect {
                val streamInfo = extractStreamInfo(Pair(it.first, it.second))

                if (streamInfo == null) {
                    println("Invalid stream info: #${it.first} - ${it.second}!")
                    return@collect
                }

                streams[streamInfo.id]?.let { existingStream ->
                    if (existingStream.path == streamInfo.path) {
                        // If this stream already exists with the same path, don't do anything.
                        return@collect
                    } else { // If it exists with another path, erase it
                        JniApi.removeStream(skywayServerHandle, streamInfo.path)
                    }
                }

                println("Adding stream #${streamInfo.id} with url: ${streamInfo.location}")
                streams[streamInfo.id] = streamInfo
                addStream(streamInfo)
            }
        }
    }

    override fun start() {
        println("Starting RTSP proxy")
        JniApi.start(skywayServerHandle)
    }

    override fun stop() {
        println("Stopping RTSP proxy")
        videoStreamFlowCollectJob?.cancel()
        JniApi.stop(skywayServerHandle)
    }

    override fun getPort(): Int {
        return JniApi.getPort(skywayServerHandle)
    }

    protected abstract fun addStream(streamInfo: StreamInfo)
}
