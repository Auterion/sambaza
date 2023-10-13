cmake_minimum_required(VERSION 3.1)

project(external-glib)
include(ExternalProject)

# Make CMAKE_INSTALL_PREFIX absolute, for Meson
get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" REALPATH)

if(${BUILD_SHARED_LIBS})
    set(LIBRARY_TYPE "shared")
else()
    set(LIBRARY_TYPE "static")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(BUILD_TYPE "debug")
else()
    set(BUILD_TYPE "release")
endif()

if(ANDROID)
    if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
        set(CROSS_FILE_OPT "--cross-file")
        set(CROSS_FILE "/work/dependencies/meson/android-armv7.cross")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
        set(CROSS_FILE_OPT "--cross-file")
        set(CROSS_FILE "/work/dependencies/meson/android-aarch64.cross")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86")
        set(CROSS_FILE_OPT "--cross-file")
        set(CROSS_FILE "/work/dependencies/meson/android-x86.cross")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86_64")
        set(CROSS_FILE_OPT "--cross-file")
        set(CROSS_FILE "/work/dependencies/meson/android-x86_64.cross")
    else()
        message(FATAL_ERROR "Unexpected PLATFORM: \"${CMAKE_ANDROID_ARCH_ABI}\"")
    endif()
endif()

ExternalProject_Add(
    glib
    URL "https://download.gnome.org/sources/glib/2.76/glib-2.76.4.tar.xz"
    PREFIX glib
    PATCH_COMMAND patch < ${PROJECT_SOURCE_DIR}/glib-iconv-pkgconfig.patch
    CONFIGURE_COMMAND meson setup --pkg-config-path=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig ${CROSS_FILE_OPT} ${CROSS_FILE} --buildtype=${BUILD_TYPE} --prefix=${CMAKE_INSTALL_PREFIX} --libdir=lib -Ddefault_library=${LIBRARY_TYPE} -Dc_args="-Wno-pointer-sign" <SOURCE_DIR>/build <SOURCE_DIR>
    BUILD_COMMAND meson compile -C <SOURCE_DIR>/build
    INSTALL_COMMAND meson install -C <SOURCE_DIR>/build
    DEPENDS iconv
)

