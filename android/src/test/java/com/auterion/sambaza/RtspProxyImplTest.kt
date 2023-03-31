package com.auterion.sambaza

import org.junit.Assert.*
import org.junit.Test

class RtspProxyImplTest {
    @Test
    fun extractStreamInfo_isCorrect() {
        val streamId = 1
        val ip = "192.168.1.4"
        val port = 8553
        val path = "/stream1"
        val location = "rtsp://$ip:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(streamId, location))

        assertNotNull(streamInfo)
        streamInfo!!

        assertEquals(streamId, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }

    @Test
    fun extractStreamInfo_isCorrectForDashedPath() {
        val streamId = 1
        val ip = "192.168.1.4"
        val port = 8553
        val path = "/a861861-dirty/stream1"
        val location = "rtsp://$ip:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(streamId, location))

        assertNotNull(streamInfo)
        streamInfo!!

        assertEquals(streamId, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }

    @Test
    fun extractStreamInfo_isStillCorrect() {
        val streamId = 1
        val ip = "1.2.3.4"
        val port = 25123
        val path = "/liiiiive"
        val location = "rtsp://$ip:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(streamId, location))

        assertNotNull(streamInfo)
        streamInfo!!

        assertEquals(streamId, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }

    @Test
    fun extractStreamInfo_worksWithRtspt() {
        val streamId = 1
        val ip = "192.168.1.4"
        val port = 8553
        val path = "/stream1"
        val location = "rtspt://$ip:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(streamId, location))

        assertNotNull(streamInfo)
        streamInfo!!

        assertEquals(streamId, streamInfo.id)
        assertEquals(ip, streamInfo.ip)
        assertEquals(port, streamInfo.port)
        assertEquals(path, streamInfo.path)
        assertEquals(location, streamInfo.location)
    }

    @Test
    fun extractStreamInfo_nullWhenIncorrectPath() {
        val streamId = 1
        val ip = "192.168.1.4"
        val port = 8553
        val path = "stream1"
        val location = "rtsp://$ip:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(streamId, location))

        assertNull(streamInfo)
    }

    @Test
    fun extractStreamInfo_nullWhenIncorrectIp() {
        val streamId = 1
        val ip = "192.168.1"
        val port = 8553
        val path = "/stream1"
        val location = "rtsp://$ip:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(streamId, location))

        assertNull(streamInfo)
    }

    @Test
    fun extractStreamInfo_nullWhenIncorrectPort() {
        val streamId = 1
        val ip = "192.168.1.66"
        val port = "not a port"
        val path = "/stream1"
        val location = "rtsp://$ip:${port}${path}"

        val streamInfo = StreamInfo.extractStreamInfo(Pair(streamId, location))

        assertNull(streamInfo)
    }
}