project(lzfse C)
cmake_minimum_required(VERSION 2.8.6)

include(CheckCCompilerFlag)

# If LZFSE is being bundled in another project, we don't want to
# install anything.  However, we want to let people override this, so
# we'll use the LZFSE_BUNDLE_MODE variable to let them do that; just
# set it to OFF in your project before you add_subdirectory(lzfse).
get_directory_property(LZFSE_PARENT_DIRECTORY PARENT_DIRECTORY)
if("${LZFSE_BUNDLE_MODE}" STREQUAL "")
  # Bundled mode hasn't been set one way or the other, set the default
  # depending on whether or not we are the top-level project.
  if(LZFSE_PARENT_DIRECTORY)
    set(LZFSE_BUNDLE_MODE ON)
  else()
    set(LZFSE_BUNDLE_MODE OFF)
  endif(LZFSE_PARENT_DIRECTORY)
endif()
mark_as_advanced(LZFSE_BUNDLE_MODE)

if(NOT LZFSE_BUNDLE_MODE)
  option(BUILD_SHARED_LIBS "Build shared libraries" ON)
else()
  set(BUILD_SHARED_LIBS OFF)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.2)
  cmake_policy (SET CMP0063 NEW)
endif ()

# Compiler flags
function(lzfse_add_compiler_flags target)
  set (flags ${ARGV})
  list (REMOVE_AT flags 0)

  foreach (FLAG ${flags})
    if(CMAKE_C_COMPILER_ID STREQUAL GNU)
      # Because https://gcc.gnu.org/wiki/FAQ#wnowarning
      string(REGEX REPLACE "\\-Wno\\-(.+)" "-W\\1" flag_to_test "${FLAG}")
    else()
      set (flag_to_test ${FLAG})
    endif()

    string(REGEX REPLACE "[^a-zA-Z0-9]+" "_" test_name "CFLAG_${flag_to_test}")

    check_c_compiler_flag("${flag_to_test}" "${test_name}")
    if(${${test_name}})
      set_property(TARGET "${target}" APPEND_STRING PROPERTY COMPILE_FLAGS " ${FLAG}")
    endif()
  endforeach()
endfunction()

if (ENABLE_SANITIZER)
  set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -fsanitize=${ENABLE_SANITIZER}")
  set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fsanitize=${ENABLE_SANITIZER}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${ENABLE_SANITIZER}")
endif ()

add_library(lzfse
  src/lzfse_decode.c
  src/lzfse_decode_base.c
  src/lzfse_encode.c
  src/lzfse_encode_base.c
  src/lzfse_fse.c
  src/lzvn_decode_base.c
  src/lzvn_encode_base.c)
lzfse_add_compiler_flags(lzfse -Wall -Wno-unknown-pragmas -Wno-unused-variable)

add_executable(lzfse_cli
  src/lzfse_main.c)
target_link_libraries(lzfse_cli lzfse)
set_target_properties(lzfse_cli PROPERTIES OUTPUT_NAME lzfse)
lzfse_add_compiler_flags(lzfse_cli -Wall -Wno-unknown-pragmas -Wno-unused-variable)

if(CMAKE_VERSION VERSION_LESS 3.1 OR CMAKE_C_COMPLIER_ID STREQUAL "Intel")
  lzfse_add_compiler_flags(lzfse -std=c99)
  lzfse_add_compiler_flags(lzfse_cli -std=c99)
else()
  set_property(TARGET lzfse PROPERTY C_STANDARD 99)
  set_property(TARGET lzfse_cli PROPERTY C_STANDARD 99)
endif()

set_target_properties(lzfse PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
  C_VISIBILITY_PRESET hidden
  INTERPROCEDURAL_OPTIMIZATION TRUE)

if(BUILD_SHARED_LIBS)
  set_property(TARGET lzfse APPEND PROPERTY COMPILE_DEFINITIONS LZFSE_DLL LZFSE_DLL_EXPORTS)
  set_property(TARGET lzfse APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS LZFSE_DLL)
endif()

# Installation
if(NOT LZFSE_BUNDLE_MODE)
  include(GNUInstallDirs)

  install(TARGETS lzfse lzfse_cli
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
  install(FILES src/lzfse.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

# Tests

# If we're targeting Windows but not running on Windows, we need Wine
# to run the tests...
if(NOT LZFSE_DISABLE_TESTS)
  if(WIN32 AND NOT CMAKE_HOST_WIN32)
    find_program(LZFSE_WINE NAMES wine)

    if(NOT LZFSE_WINE)
      message(STATUS "wine not found, disabling tests")
      set(LZFSE_DISABLE_TESTS TRUE)
    endif()
  endif()
endif()

if(NOT LZFSE_DISABLE_TESTS)
  include(CTest)
  enable_testing()

  file(GLOB_RECURSE
    ROUNDTRIP_INPUTS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*)

  foreach(INPUT ${ROUNDTRIP_INPUTS})
    add_test(NAME "${LZFSE_TEST_PREFIX}roundtrip/${INPUT}"
      COMMAND "${CMAKE_COMMAND}"
        -DLZFSE_WRAPPER=${LZFSE_WINE}
        -DLZFSE_CLI=$<TARGET_FILE:lzfse_cli>
        -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/${INPUT}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/round-trip.cmake)
  endforeach()
endif()
