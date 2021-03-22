get_filename_component(INPUT_NAME "${INPUT}" NAME)

set(LZFSE_TMP_DIR     "${CMAKE_CURRENT_BINARY_DIR}/test-tmp")
set(COMPRESSED_FILE   "${LZFSE_TMP_DIR}/${INPUT_NAME}.lzfse")
set(DECOMPRESSED_FILE "${LZFSE_TMP_DIR}/${INPUT_NAME}")

file(MAKE_DIRECTORY "${LZFSE_TMP_DIR}")

if(EXISTS COMPRESSED_FILE)
	message(FATAL_ERROR "${COMPRESSED_FILE} exists")
	file(REMOVE ${COMPRESSED_FILE})
endif()
if(EXISTS DECOMPRESSED_FILE)
	message(FATAL_ERROR "${DECOMPRESSED_FILE} exists")
	file(REMOVE ${DECOMPRESSED_FILE})
endif()

execute_process(
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
  COMMAND ${LZFSE_WRAPPER} ${LZFSE_CLI} -encode -i ${INPUT} -o ${COMPRESSED_FILE}
  RESULT_VARIABLE result
  ERROR_VARIABLE result_stderr)
if(result)
  message(FATAL_ERROR "Compression failed: ${result_stderr}")
endif()

execute_process(
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
  COMMAND ${LZFSE_WRAPPER} ${LZFSE_CLI} -decode -i ${COMPRESSED_FILE} -o ${DECOMPRESSED_FILE}
  RESULT_VARIABLE result)
if(result)
  message(FATAL_ERROR "Decompression failed")
endif()

function(test_file_equality f1 f2)
  if(NOT CMAKE_VERSION VERSION_LESS 2.8.7)
    file(SHA512 "${f1}" f1_cs)
    file(SHA512 "${f2}" f2_cs)
    if(NOT "${f1_cs}" STREQUAL "${f2_cs}")
      message(FATAL_ERROR "Files do not match")
    endif()
  else()
    file(READ "${f1}" f1_contents)
    file(READ "${f2}" f2_contents)
    if(NOT "${f1_contents}" STREQUAL "${f2_contents}")
      message(FATAL_ERROR "Files do not match")
    endif()
  endif()
endfunction()

test_file_equality("${INPUT}" "${DECOMPRESSED_FILE}")
file(REMOVE ${COMPRESSED_FILE})
file(REMOVE ${DECOMPRESSED_FILE})
