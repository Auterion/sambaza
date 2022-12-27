package com.auterion.rtsp_proxy

class H264Frame(val pts: ULong, val caps: String?) {
    private var buffer = ByteArray(0)

    fun buffer(): ByteArray {
        return buffer
    }

    fun setBuffer(value: ByteArray) {
        buffer = value
    }
}
