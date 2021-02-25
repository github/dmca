add_library(librw_skeleton_imgui
    imconfig.h
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui.h
    imgui_impl_rw.cpp
    imgui_impl_rw.h
    imgui_internal.h
    ImGuizmo.cpp
    ImGuizmo.h
    stb_rect_pack.h
    stb_textedit.h
    stb_truetype.h
)

set_target_properties(librw_skeleton_imgui
    PROPERTIES
        PREFIX ""
)

target_link_libraries(librw_skeleton_imgui
    PUBLIC
        librw_skeleton
)

if(LIBRW_INSTALL)
    install(
        FILES
            imconfig.h
            imgui.h
            imgui_impl_rw.h
            imgui_internal.h
            ImGuizmo.h
            stb_textedit.h
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}/skeleton/imgui"
    )

    install(
        TARGETS librw_skeleton_imgui
        EXPORT librw-targets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()
