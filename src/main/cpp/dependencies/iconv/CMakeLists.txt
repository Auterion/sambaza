cmake_minimum_required(VERSION 3.1)

project(external-iconv)
include(ExternalProject)

set(ICONV_VERSION "1.17")

if(${BUILD_SHARED_LIBS})
    set(BUILD_SHARED "--enable-shared")
    set(BUILD_STATIC "--disable-static")
else()
    set(BUILD_SHARED "--disable-shared")
    set(BUILD_STATIC "--enable-static")
endif()

if(ANDROID)
    if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
        message(FATAL_ERROR "Not implemented")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
        set(HOST_OPT "--host")
        set(HOST "aarch64-linux-android")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86")
        message(FATAL_ERROR "Not implemented")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86_64")
        message(FATAL_ERROR "Not implemented")
    else()
        message(FATAL_ERROR "Unexpected PLATFORM: \"${CMAKE_ANDROID_ARCH_ABI}\"")
    endif()
endif()

# Create my-iconv.pc, to be used by the glib build. It needs to be called
# something other than `iconv.pc`, otherwise Meson goes into the "specific
# detection logic" which is hardcoded to not use pkg-config. This workaround
# also requires a patch in glib such that it looks for `my-iconv.pc`.
configure_file(my-iconv.pc.in
    "${PROJECT_BINARY_DIR}/my-iconv.pc" @ONLY)

ExternalProject_Add(
    iconv
    URL "https://ftp.gnu.org/pub/gnu/libiconv/libiconv-${ICONV_VERSION}.tar.gz"
    PREFIX iconv
    CONFIGURE_COMMAND <SOURCE_DIR>/configure ${HOST_OPT} ${HOST} ${BUILD_SHARED} ${BUILD_STATIC} --prefix=${CMAKE_INSTALL_PREFIX}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
    COMMAND cp ${PROJECT_BINARY_DIR}/my-iconv.pc ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/my-iconv.pc
)

