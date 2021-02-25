include(${CMAKE_ROOT}/Modules/CMakeDetermineCompiler.cmake)

if (NOT CMAKE_DSM_COMPILER)
  message(FATAL_ERROR "Need CMAKE_DSM_COMPILER set")
endif()

_cmake_find_compiler_path(DSM)
mark_as_advanced(CMAKE_DSM_COMPILER)

if (NOT CMAKE_DSM_COMPILER_ID)
  # Table of per-vendor compiler id flags with expected output.
  list(APPEND CMAKE_DSM_COMPILER_ID_VENDORS GNU )
  set(CMAKE_DSM_COMPILER_ID_VENDOR_FLAGS_GNU "--version")
  set(CMAKE_DSM_COMPILER_ID_VENDOR_REGEX_GNU "(GNU assembler)|(GCC)|(Free Software Foundation)")

  include(CMakeDetermineCompilerId)
  cmake_determine_compiler_id_vendor(DSM "")

endif()

if (NOT _CMAKE_TOOLCHAIN_LOCATION)
  get_filename_component(_CMAKE_TOOLCHAIN_LOCATION "${CMAKE_DSM_COMPILER}" PATH)
endif()

if (CMAKE_DSM_COMPILER_ID)
  if (CMAKE_DSM_COMPILER_VERSION)
    set(_version " ${CMAKE_DSM_COMPILER_VERSION}")
  else()
    set(_version "")
  endif()
  message(STATUS "The DSM compiler identification is ${CMAKE_DSM_COMPILER_ID}${_version}")
  unset(_version)
else()
  message(STATUS "The DSM compiler identification is unknown")
endif()

if (NOT _CMAKE_TOOLCHAIN_PREFIX)
  get_filename_component(COMPILER_BASENAME "${CMAKE_DSM_COMPILER}" NAME)
  if (COMPILER_BASENAME MATCHES "^(.+1)g?as(-[0-9]+\\.[0-9]+\\.[0-9]+)?(\\.exe)?$")
    set(_CMAKE_TOOLCHAIN_PREFIX ${CMAKE_MATCH_1})
  endif()

endif()

set(_CMAKE_PROCESSING_LANGUAGE "DSM")
find_program(CMAKE_DSM_COMPILER_AR NAMES ${_CMAKE_TOOLCHAIN_PREFIX}ar HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
find_program(CMAKE_DSM_COMPILER_RANLIB NAMES ${_CMAKE_TOOLCHAIN_PREFIX}ranlib HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
find_program(CMAKE_DSM_COMPILER_STRIP NAMES ${_CMAKE_TOOLCHAIN_PREFIX}strip HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
find_program(CMAKE_DSM_COMPILER_NM NAMES ${_CMAKE_TOOLCHAIN_PREFIX}nm HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
find_program(CMAKE_DSM_COMPILER_OBJDUMP NAMES ${_CMAKE_TOOLCHAIN_PREFIX}objdump HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
find_program(CMAKE_DSM_COMPILER_OBJCOPY NAMES ${_CMAKE_TOOLCHAIN_PREFIX}objcopy HINTS ${_CMAKE_TOOLCHAIN_LOCATION})

unset(_CMAKE_PROCESSING_LANGUAGE)

set(CMAKE_DSM_COMPILER_ENV_VAR "DSM")

if (CMAKE_DSM_COMPILER)
  message(STATUS "Found DSM assembler: ${CMAKE_DSM_COMPILER}")
else()
  message(STATUS "Didn't find assembler")
endif()

foreach(_var
    COMPILER
    COMPILER_ID
    COMPILER_ARG1
    COMPILER_ENV_VAR
    COMPILER_AR
    COMPILER_RANLIB
    COMPILER_VERSION
    )
  set(_CMAKE_DSM_${_var} "${CMAKE_DSM_${_var}}")
endforeach()

configure_file("${CMAKE_CURRENT_LIST_DIR}/CMakeDSMCompiler.cmake.in"
    "${CMAKE_PLATFORM_INFO_DIR}/CMakeDSMCompiler.cmake" @ONLY)

foreach(_var
    COMPILER
    COMPILER_ID
    COMPILER_ARG1
    COMPILER_ENV_VAR
    COMPILER_AR
    COMPILER_VERSION
  )
  unset(_CMAKE_DSM_${_var})
endforeach()
