cmake_minimum_required(VERSION 3.4.1)

project(rtsp-proxy C)

if (MSVC)
    add_compile_options(/WX /W2)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${CMAKE_ANDROID_ARCH_ABI}/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${CMAKE_ANDROID_ARCH_ABI}/lib/gstreamer-1.0
)

add_library(rtsp_proxy SHARED
        appsink_proxy.c
        appsrc_factory.c
        gstbuffer_to_sink.c
        rtsp_server.c
        rtspsrc_to_sink.c
        rtsp_proxy_jni_api.c)

target_include_directories(rtsp_proxy SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${CMAKE_ANDROID_ARCH_ABI}/include/gstreamer-1.0
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${CMAKE_ANDROID_ARCH_ABI}/include/glib-2.0
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${CMAKE_ANDROID_ARCH_ABI}/lib/glib-2.0/include
        )

target_link_libraries(rtsp_proxy
        gstrtsp
        gstrtp
        gstvideo-1.0
        gstaudio-1.0
        gstpbutils-1.0
        gsttag-1.0
        orc-0.4
        gstrtpmanager
        gsttcp
        gstudp
        gstcoreelements
        gstrtspserver-1.0
        gstbase-1.0
        gstreamer-1.0
        intl
        gstrtsp-1.0
        gstnet-1.0
        gstsdp-1.0
        gio-2.0
        gstapp-1.0
        gstrtp-1.0
        gmodule-2.0
        ffi
        iconv
        glib-2.0
        gobject-2.0
        z
        gstbase-1.0
        gstapp
        gstvideoparsersbad
        gstcodecparsers-1.0
        gstvideo-1.0
        log
        )
