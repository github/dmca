add_executable(imguitest WIN32
    main.cpp
)

target_link_libraries(imguitest
    PUBLIC
        librw
        librw_skeleton_imgui
)

if(LIBRW_INSTALL)
    install(TARGETS imguitest
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
endif()
