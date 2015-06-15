cmake_minimum_required(VERSION 2.8)
set(LIBVLCPP_MAJOR_VERSION 2)
set(LIBVLCPP_MINOR_VERSION 2)
set(LIBVLCPP_PATCH_VERSION 0)
set(LIBVLCPP_VERSION ${LIBVLCPP_MAJOR_VERSION}.${LIBVLCPP_MINOR_VERSION}.${LIBVLCPP_PATCH_VERSION})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include_directories("${CMAKE_SOURCE_DIR}")
find_package(LIBVLC REQUIRED)

include_directories("${LIBVLC_INCLUDE_DIR}")

subdirs(examples)
subdirs(test)

include(cpp11)
add_definitions("-Wall -Wextra -pedantic")
EnableCpp11()

file(GLOB LIBVLCPP_HEADERS "${CMAKE_SOURCE_DIR}/vlcpp/*.hpp")


