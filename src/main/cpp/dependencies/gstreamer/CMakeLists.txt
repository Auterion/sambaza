cmake_minimum_required(VERSION 3.1)

project(external-gstreamer)
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
    set(CROSS_FILE_OPT "--cross-file")
    set(CROSS_FILE "/work/dependencies/meson/android.cross") # NOTE: hardcoded for dockcross

    set(A64_NEON "disabled")
else()
    set(A64_NEON "auto")
endif()

ExternalProject_Add(
    gstreamer
    URL "https://gitlab.freedesktop.org/gstreamer/gstreamer/-/archive/1.20.5/gstreamer-1.20.5.tar.gz"
    PREFIX gstreamer
    CONFIGURE_COMMAND meson setup --pkg-config-path=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig ${CROSS_FILE_OPT} ${CROSS_FILE} --buildtype=${BUILD_TYPE} --prefix=${CMAKE_INSTALL_PREFIX} --libdir=lib -Ddefault_library=${LIBRARY_TYPE} -Dpython=disabled -Dqt5=disabled -Dges=disabled -Dnls=disabled -Dgst-examples=disabled -Dexamples=disabled -Dtests=disabled -Ddoc=disabled -Dgtk_doc=disabled -Dorc:benchmarks=disabled -Dorc:examples=disabled -Dorc:gtk_doc=disabled -Dorc:tests=disabled -Dorc:tools=disabled -Dorc:orc-test=disabled -Dgst-plugins-bad:fdkaac=disabled -Dpixman:a64-neon=${A64_NEON} <SOURCE_DIR>/build <SOURCE_DIR>
    BUILD_COMMAND meson compile -C <SOURCE_DIR>/build
    INSTALL_COMMAND meson install -C <SOURCE_DIR>/build
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-1.0/pkgconfig ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
    DEPENDS glib
)

# Note about the post-install COMMAND:
# ------------------------------------
#
# Our main project depending on gstreamer is using pkg-config to find 
# the install gstreamer libraries. In CMake, the PKG_CONFIG_PATH is
# populated from different variables including CMAKE_PREFIX_PATH, and
# then the path to pkgconfig/ is somehow hardcoded [1].
#
# That works fine when gstreamer is built as shared libs, but not when
# built as static libs, because gstreamer installs "internal" .pc files
# in lib/gstreamer-1.0/pkgconfig. Therefore I copy all those "internal"
# .pc files into lib/pkgconfig.
#
# A cleaner approach could be to write a CMake module (FindGstreamer)
# that would find the internal pkgconfig/ and add it to PKG_CONFIG_PATH
# (e.g. with `set(ENV{PKG_CONFIG_PATH} <path>)`), but this will do for
# now.
#
# [1]: https://github.com/Kitware/CMake/blob/0038a7155493221e36cdc4a5c1ca11b9eb158d43/Modules/FindPkgConfig.cmake#L344-L435
