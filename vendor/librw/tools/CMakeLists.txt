if(NOT LIBRW_PLATFORM_PS2)
    add_subdirectory(dumprwtree)
endif()

if(TARGET librw_skeleton_imgui)
    add_subdirectory(imguitest)
endif()

if(LIBRW_PLATFORM_PS2)
    add_subdirectory(ps2test)
endif()
