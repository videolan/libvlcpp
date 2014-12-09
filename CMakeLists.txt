project(vlcpp)
cmake_minimum_required(VERSION 2.8)
set(LIBVLCPP_MAJOR_VERSION 2)
set(LIBVLCPP_MINOR_VERSION 2)
set(LIBVLCPP_PATCH_VERSION 0)
set(LIBVLCPP_VERSION ${LIBVLCPP_MAJOR_VERSION}.${LIBVLCPP_MINOR_VERSION}.${LIBVLCPP_PATCH_VERSION})

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
foreach(p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

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

file(GLOB LIBVLCPP_PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/src/*.hpp")
set_target_properties(vlcpp PROPERTIES
    PUBLIC_HEADER "${LIBVLCPP_PUBLIC_HEADERS}"
)

find_package(LIBVLC REQUIRED)
target_link_libraries( ${PROJECT_NAME} ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY} )
include_directories(${LIBVLC_INCLUDE_DIR})

file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "\${LIBVLCPP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(${CMAKE_SOURCE_DIR}/cmake/vlcppConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/vlcppConfig.cmake" @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/cmake/vlcppConfigVersion.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/vlcppConfigVersion.cmake" @ONLY)

install(
    FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/vlcppConfig.cmake" "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/vlcppConfigVersion.cmake"
    DESTINATION lib/${PROJECT_NAME}
    COMPONENT dev
)

install(TARGETS vlcpp
    LIBRARY DESTINATION lib/${PROJECT_NAME}
    ARCHIVE DESTINATION lib/${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
    COMPONENT dev
)

