cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

set(CMAKE_SYSTEM_NAME "PlayStation2")
set(CMAKE_SYSTEM_PROCESSOR "mipsel")
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT DEFINED ENV{PS2DEV})
    message(FATAL_ERROR "Need environment variable PS2DEV set")
endif()
if(NOT DEFINED ENV{PS2SDK})
    message(FATAL_ERROR "Need environment variable PS2SDK set")
endif()
set(PS2DEV "$ENV{PS2DEV}")
set(PS2SDK "$ENV{PS2SDK}")

set(CMAKE_DSM_SOURCE_FILE_EXTENSIONS "dsm")

set(CMAKE_C_COMPILER "${PS2DEV}/ee/bin/ee-gcc")
set(CMAKE_CXX_COMPILER "${PS2DEV}/ee/bin/ee-g++")
set(CMAKE_ASM_COMPILER "${PS2DEV}/ee/bin/ee-g++")
set(CMAKE_DSM_COMPILER "${PS2DEV}/dvp/bin/dvp-as")
set(CMAKE_AR "${PS2DEV}/ee/bin/ee-ar" CACHE FILEPATH "archiver")
set(CMAKE_LINKER "${PS2DEV}/ee/bin/ee-ld")
set(CMAKE_RANLIB "${PS2DEV}/ee/bin/ee-ranlib" CACHE FILEPATH "ranlib")
set(CMAKE_STRIP "${PS2DEV}/ee/bin/ee-strip" CACHE FILEPATH "strip")

set(CMAKE_ASM_FLAGS_INIT "-G0 -I\"${PS2SDK}/ee/include\" -I\"${PS2SDK}/common/include\"")
set(CMAKE_C_FLAGS_INIT "-G0 -fno-common -I\"${PS2SDK}/ee/include\" -I\"${PS2SDK}/common/include\"")
set(CMAKE_CXX_FLAGS_INIT "-G0 -fno-common -I\"${PS2SDK}/ee/include\" -I\"${PS2SDK}/common/include\"")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-G0 -L\"${PS2SDK}/ee/lib\" -Wl,-r -Wl,-d")

set(CMAKE_FIND_ROOT_PATH "${PS2DEV}/ee;${PS2SDK}/ee")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(PS2 1)
set(EE 1)

function(add_erl_executable OUTFILE TARGET)
    get_property(output_dir TARGET ${TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY)
    set(outfile "${output_dir}/${TARGET}.erl")
    add_custom_command(OUTPUT "${outfile}"
        COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:${TARGET}>" "${outfile}"
        COMMAND "${CMAKE_STRIP}" --strip-unneeded -R .mdebug.eabi64 -R .reginfo -R .comment "${outfile}"
        DEPENDS ${TARGET}
    )
    add_custom_target("${TARGET}_erl" ALL
        DEPENDS "${outfile}"
    )
    set("${OUTFILE}" "${outfile}" PARENT_SCOPE)
endfunction()
