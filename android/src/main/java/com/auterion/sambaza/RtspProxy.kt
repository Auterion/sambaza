package com.auterion.sambaza

interface RtspProxy {
    fun start()
    fun stop()
    fun getPort(): Int
}