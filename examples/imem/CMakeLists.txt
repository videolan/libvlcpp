project(imem)

add_executable(${PROJECT_NAME}
    imem.cpp
)
target_link_libraries( ${PROJECT_NAME} ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY} )
