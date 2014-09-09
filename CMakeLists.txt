project(libvlcpp)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
add_definitions("-Wall -Wextra -pedantic")

add_library (${PROJECT_NAME} SHARED
    src/Media.cpp
    src/EventManager.cpp
    src/MediaLibrary.cpp
    src/MediaListPlayer.cpp
    src/Instance.cpp
    src/MediaDiscoverer.cpp
    src/MediaList.cpp
    src/MediaPlayer.cpp
    src/structures.cpp
)

find_package(LIBVLC REQUIRED)
target_link_libraries( ${PROJECT_NAME} ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY} )
include_directories(${LIBVLC_INCLUDE_DIR})
