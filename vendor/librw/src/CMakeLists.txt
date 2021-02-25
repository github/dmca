add_library(librw
    "${PROJECT_SOURCE_DIR}/args.h"
    "${PROJECT_SOURCE_DIR}/rw.h"

    anim.cpp
    base.cpp
    bmp.cpp
    camera.cpp
    charset.cpp
    clump.cpp
    engine.cpp
    error.cpp
    frame.cpp
    geometry.cpp
    geoplg.cpp
    hanim.cpp
    image.cpp
    light.cpp
    matfx.cpp
    pipeline.cpp
    plg.cpp
    png.cpp
    prim.cpp
    raster.cpp
    render.cpp
    rwanim.h
    rwengine.h
    rwerror.h
    rwobjects.h
    rwpipeline.h
    rwplg.h
    rwplugins.h
    rwrender.h
    rwuserdata.h
    skin.cpp
    texture.cpp
    tga.cpp
    tristrip.cpp
    userdata.cpp
    uvanim.cpp
    world.cpp

    d3d/d3d8.cpp
    d3d/d3d8matfx.cpp
    d3d/d3d8render.cpp
    d3d/d3d8skin.cpp
    d3d/d3d9.cpp
    d3d/d3d9matfx.cpp
    d3d/d3d9render.cpp
    d3d/d3d9skin.cpp
    d3d/d3d.cpp
    d3d/d3ddevice.cpp
    d3d/d3dimmed.cpp
    d3d/d3drender.cpp
    d3d/rwd3d8.h
    d3d/rwd3d9.h
    d3d/rwd3d.h
    d3d/rwd3dimpl.h
    d3d/rwxbox.h
    d3d/rwxboximpl.h
    d3d/xbox.cpp
    d3d/xboxmatfx.cpp
    d3d/xboxskin.cpp
    d3d/xboxvfmt.cpp

    gl/gl3.cpp
    gl/gl3device.cpp
    gl/gl3immed.cpp
    gl/gl3matfx.cpp
    gl/gl3pipe.cpp
    gl/gl3raster.cpp
    gl/gl3render.cpp
    gl/gl3shader.cpp
    gl/gl3skin.cpp
    gl/rwgl3.h
    gl/rwgl3impl.h
    gl/rwgl3plg.h
    gl/rwgl3shader.h
    gl/rwwdgl.h
    gl/wdgl.cpp
    gl/glad/glad.h
    gl/glad/glad.c
    gl/glad/khrplatform.h

    lodepng/lodepng.h
    lodepng/lodepng.cpp

    ps2/pds.cpp
    ps2/ps2.cpp
    ps2/ps2device.cpp
    ps2/ps2matfx.cpp
    ps2/ps2raster.cpp
    ps2/ps2skin.cpp
    ps2/rwps2.h
    ps2/rwps2impl.h
    ps2/rwps2plg.h
)
add_library(librw::librw ALIAS librw)

target_include_directories(librw
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)

target_compile_definitions(librw
    PRIVATE
        LODEPNG_NO_COMPILE_CPP
        "$<IF:$<CONFIG:DEBUG>,DEBUG,NDEBUG>"
    PUBLIC
        "RW_${LIBRW_PLATFORM}"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(librw
        PRIVATE
            m
    )
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_options(librw
        PRIVATE
            "-Wall"
    )
    if (NOT LIBRW_PLATFORM_PS2)
        target_compile_options(librw
            PRIVATE
                "-Wextra"
                "-Wdouble-promotion"
                "-Wpedantic"
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(librw
        PUBLIC
            /wd4996 /wd4244
    )
endif()

set_target_properties(librw
    PROPERTIES
        C_STANDARD 11
        C_EXTENSIONS OFF
        C_STANDARD_REQUIRED ON
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        PREFIX ""
)

if(LIBRW_PLATFORM_GL3)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)

    target_link_libraries(librw
        PUBLIC
            OpenGL::GL
    )
    if (LIBRW_GL3_GFXLIB STREQUAL "GLFW")
        find_package(glfw3 REQUIRED)
        target_link_libraries(librw
            PUBLIC
                glfw
            )
    elseif (LIBRW_GL3_GFXLIB STREQUAL "SDL2")
        find_package(SDL2 REQUIRED)
        target_compile_definitions(librw PUBLIC LIBRW_SDL2)
        target_link_libraries(librw
            PUBLIC
                SDL2::SDL2
        )
        if(TARGET SDL2::SDL2main)
            target_link_libraries(librw
                PUBLIC
                    SDL2::SDL2main
            )
        endif()
    endif()
elseif(LIBRW_PLATFORM_D3D9)
    target_link_libraries(librw
        PUBLIC
            d3d9
            xinput
    )
endif()

if(LIBRW_INSTALL)
    target_include_directories(librw
        INTERFACE
            $<INSTALL_INTERFACE:${LIBRW_INSTALL_INCLUDEDIR}>
    )

    install(
        FILES
            "${PROJECT_SOURCE_DIR}/args.h"
            "${PROJECT_SOURCE_DIR}/rw.h"
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}"
    )
    install(
        FILES
            base.err
            rwbase.h
            rwcharset.h
            rwerror.h
            rwplg.h
            rwrender.h
            rwengine.h
            rwpipeline.h
            rwobjects.h
            rwanim.h
            rwplugins.h
            rwuserdata.h
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}/src"
    )
    install(
        FILES
            d3d/rwxbox.h
            d3d/rwd3d.h
            d3d/rwd3d8.h
            d3d/rwd3d9.h
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}/src/d3d"
    )
    install(
        FILES
            ps2/rwps2.h
            ps2/rwps2plg.h
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}/src/ps2"
    )
    install(
        FILES
            gl/rwwdgl.h
            gl/rwgl3.h
            gl/rwgl3plg.h
            gl/rwgl3shader.h
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}/src/gl"
    )

    install(
        FILES
            gl/glad/glad.h
            gl/glad/khrplatform.h
        DESTINATION "${LIBRW_INSTALL_INCLUDEDIR}/src/gl/glad"
    )

    install(
        TARGETS librw
        EXPORT librw-targets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()
