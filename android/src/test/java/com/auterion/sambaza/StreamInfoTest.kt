package com.auterion.sambaza

import org.junit.Assert
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Test

class StreamInfoTest {
    @Test
    fun extractStreamInfo_isCorrectForStandardStream() {
        val id = 1
        val ip = "192.168.1.12"
        val port = 8554
        val path = "/stream1"
        val location = "rtsp://${ip}:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(id, location))!!

        assertEquals(id, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }

    @Test
    fun extractStreamInfo_isCorrectForArbitraryStreamId() {
        val id = 58
        val ip = "192.168.1.12"
        val port = 8554
        val path = "/stream1"
        val location = "rtsp://${ip}:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(id, location))!!

        assertEquals(id, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }

    @Test
    fun extractStreamInfo_isCorrectForWeirdPath() {
        val id = 1
        val ip = "192.168.1.12"
        val port = 8554
        val path = "/a861861-dirty/stream1"
        val location = "rtsp://${ip}:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(id, location))!!

        assertEquals(id, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }

    @Test
    fun extractStreamInfo_isCorrectForRtspt() {
        val id = 1
        val ip = "192.168.1.12"
        val port = 8554
        val path = "/stream1"
        val location = "rtspt://${ip}:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(id, location))!!

        assertEquals(id, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }
}
