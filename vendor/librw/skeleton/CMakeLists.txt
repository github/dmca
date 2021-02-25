add_library(librw_skeleton
        glfw.cpp
        sdl2.cpp
        skeleton.cpp
        skeleton.h
        win.cpp
)

set_target_properties(librw_skeleton
    PROPERTIES
        PREFIX ""
)

target_link_libraries(librw_skeleton
    PUBLIC
        librw
)

target_include_directories(librw_skeleton
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if(NOT LIBRW_PLATFORM_PS2 AND NOT LIBRW_PLATFORM_NULL)
    add_subdirectory(imgui)
endif()

if(LIBRW_INSTALL)
    target_include_directories(librw_skeleton
        INTERFACE
            $<INSTALL_INTERFACE:${LIBRW_INSTALL_INCLUDEDIR}/skeleton>
    )

    install(
        FILES
            skeleton.h
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}/skeleton"
    )

    install(
        TARGETS librw_skeleton
        EXPORT librw-targets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()
