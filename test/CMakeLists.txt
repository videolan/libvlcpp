project(vlcpptest)

file(GLOB LIBVLCPP_HEADERS "${CMAKE_SOURCE_DIR}/vlcpp/*.hpp")
add_executable(${PROJECT_NAME}
    main.cpp
# Forcing cmake to load & display libvlcpp files
    ${LIBVLCPP_HEADERS}
)
target_link_libraries( ${PROJECT_NAME} ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY} )
option(WITH_ASAN "Build with address sanitizer" OFF)

if(${WITH_ASAN})
    add_definitions("-fsanitize=address -fno-omit-frame-pointer")
    target_link_libraries(${PROJECT_NAME} "asan")
endif()
