cmake_minimum_required(VERSION 3.16)
project(SIM2022)

include(cmake/list_dirs.cmake)
include(cmake/upd_tar_list.cmake)
include(cmake/comp_flags.cmake)
include(cmake/clang_format.cmake)

# indicate the docs build
option(BUILD_DOC "Build docs" OFF)
# indicate the tests build
option(BUILD_TESTS "Build tests" ON)
# add -Werror option
option(ENABLE_WERROR "Enable -Werror option (CI)" OFF)

# Test running stuff
if (BUILD_TESTS)
  enable_testing()
endif()

find_package(spdlog REQUIRED)
message("Found package: spdlog")

add_subdirectory(docs)
add_subdirectory(src)
add_subdirectory(tools)

add_subdirectory(test)
add_subdirectory(thirdparty)

message("Collected libs: ${LIBLIST}")
message("Collected tools: ${TOOLLIST}")

set(TARGETS)
list(APPEND TARGETS ${LIBLIST} ${TOOLLIST})

foreach(TARGET ${TARGETS})
  target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_include_directories(${TARGET} PUBLIC ${CMAKE_BINARY_DIR}/include/codegen)
  add_dependencies(${TARGET} DecoderGenerator)
  target_compile_features(${TARGET} PRIVATE cxx_std_20)
  target_link_libraries(${TARGET} PRIVATE spdlog::spdlog)
  apply_compiler_flags(${TARGET} PUBLIC)
  if (ENABLE_WERROR)
    target_compile_options(${TARGET} PRIVATE -Werror)
  endif()
endforeach()

foreach(TOOL ${TOOLLIST})
  target_link_libraries(${TOOL} PRIVATE ${LIBLIST})
endforeach()

