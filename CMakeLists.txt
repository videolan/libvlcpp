project(vlcpptest)
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
include(cpp11)
add_definitions("-Wall -Wextra -pedantic")
EnableCpp11()

add_executable(${PROJECT_NAME}
    test/main.cpp
)

include_directories("${CMAKE_SOURCE_DIR}/src/")

file(GLOB LIBVLCPP_PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/src/*.hpp")
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${LIBVLCPP_PUBLIC_HEADERS}"
)

find_package(LIBVLC REQUIRED)
target_link_libraries( ${PROJECT_NAME} ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY} )
include_directories(${LIBVLC_INCLUDE_DIR})

file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "\${LIBVLCPP_CMAKE_DIR}/${REL_INCLUDE_DIR}")


