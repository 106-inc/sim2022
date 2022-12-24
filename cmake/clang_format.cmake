if(NOT SKIP_CLANG_FORMAT)
  find_program(CLANG_FORMAT clang-format HINTS ${CLANG_FORMAT_PATH})

  if(NOT CLANG_FORMAT)
    message(STATUS "Probably need to specify CLANG_FORMAT_PATH")
    message(FATAL_ERROR "Clang-format not found")
  endif()
endif()

function(
  format_custom_target
  TARGET
  PREFIX
  SOURCE_LIST
  CHECK_INC
  TAR_NAME
  DEP_TARGET)
  if(SKIP_CLANG_FORMAT)
    return()
  endif()

  set(CFNAME "clangformat_${TAR_NAME}")
  set(SRCS "")
  foreach(SOURCE_FILE ${SOURCE_LIST})
    list(APPEND SRCS "${PREFIX}/${SOURCE_FILE}")
  endforeach()

  if(CHECK_INC)
    set(INCDIR ${CMAKE_SOURCE_DIR}/include/${TARGET})
    file(GLOB HEADERSRCS ${INCDIR}/*)
    foreach(SOURCE_FILE ${HEADERSRCS})
      list(APPEND SRCS "${SOURCE_FILE}")
    endforeach()
  endif()

  add_custom_target(${CFNAME} COMMAND ${CLANG_FORMAT} -i ${SRCS})
  add_dependencies(${TARGET} ${CFNAME})

  if(NOT DEP_TARGET STREQUAL "")
    add_dependencies(${CFNAME} "${DEP_TARGET}")
  endif()
endfunction()

function(format_target TARGET PREFIX SOURCE_LIST)
  format_custom_target(${TARGET} ${PREFIX} "${SOURCE_LIST}" TRUE ${TARGET} "")
endfunction()

function(format_sources TARGET PREFIX SOURCE_LIST TAR_NAME DEP_TARGET)
  format_custom_target("${TARGET}" "${PREFIX}" "${SOURCE_LIST}" FALSE
                       "${TAR_NAME}" "${DEP_TARGET}")
endfunction()

macro(add_format_exec TARGET_NAME SOURCE_LIST)
  add_executable(${TARGET_NAME} ${SOURCE_LIST})
  format_custom_target(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR} "${SOURCE_LIST}" FALSE ${TARGET_NAME} "")
endmacro()
