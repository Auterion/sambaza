package com.auterion.sambaza

data class StreamInfo(
    val id: Int, // e.g. 1
    val location: String, // e.g. rtsp://192.168.1.12:8553/stream1
    val ip: String, // e.g. 192.168.1.12
    val port: Int, // e.g. 8553
    val path: String // e.g. /stream1
) {
    companion object {
        fun extractStreamInfo(info: Pair<Int, String>): StreamInfo? {
            val srcRegex = "rtspt?://((\\d+\\.){3}\\d+):(\\d+)(/[a-z0-9/-]+)".toRegex()
            val match = srcRegex.find(info.second) ?: return null

            val ip = match.groups[1]?.value ?: return null
            val port = match.groups[3]?.value ?: return null
            val path = match.groups[4]?.value ?: return null

            return StreamInfo(
                info.first,
                info.second,
                ip,
                port.toInt(),
                path
            )
        }
    }
}