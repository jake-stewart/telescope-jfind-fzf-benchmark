include(ExternalProject)

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    set(LIBVTERM_BUILD_COMMAND "gmake")
else()
    set(LIBVTERM_BUILD_COMMAND "make")
endif()

find_program(LIBTOOL NAMES libtool glibtool)
if(NOT LIBTOOL)
    message(FATAL_ERROR "libtool not found. Please install libtool")
endif()

ExternalProject_add(libvterm
    GIT_REPOSITORY https://github.com/Sbozzolo/libvterm-mirror.git
    GIT_TAG 64f1775952dbe001e989f2ab679563b54f2fca55
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${LIBVTERM_BUILD_COMMAND} "CFLAGS='-fPIC'" "LDFLAGS='-static'"
    BUILD_IN_SOURCE ON
    INSTALL_COMMAND "")

ExternalProject_Get_property(libvterm SOURCE_DIR)

set(LIBVTERM_INCLUDE_DIR ${SOURCE_DIR}/include)
set(LIBVTERM_LIBRARY ${SOURCE_DIR}/.libs/libvterm.a)

file(MAKE_DIRECTORY ${LIBVTERM_INCLUDE_DIR})
add_library(vterm STATIC IMPORTED)
set_target_properties(vterm PROPERTIES IMPORTED_LOCATION ${LIBVTERM_LIBRARY})
target_include_directories(vterm INTERFACE ${LIBVTERM_INCLUDE_DIR})

cmake_minimum_required(VERSION 3.10)

project(benchmark-vterm)

set(CMAKE_BUILD_TYPE Release)

file(GLOB SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)

add_executable(benchmark-vterm ${SOURCES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(benchmark-vterm PRIVATE Threads::Threads)
target_link_libraries(benchmark-vterm PUBLIC vterm)
