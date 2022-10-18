cmake_minimum_required(VERSION 3.15)
project(SIM2022)

include(cmake/list_dirs.cmake)
include(cmake/comp_flags.cmake)
include(cmake/clang_format.cmake)

find_package(spdlog REQUIRED)
message("Found package: spdlog")

add_subdirectory(src)
add_subdirectory(tools)

add_subdirectory(test)
add_subdirectory(thirdparty)

set(TARGETS)
list(APPEND TARGETS ${LIBLIST} ${TOOLLIST})

foreach(TARGET ${TARGETS})
  target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_compile_features(${TARGET} INTERFACE cxx_std_20)
  target_link_libraries(${TARGET} PRIVATE spdlog::spdlog)
  apply_compiler_flags(${TARGET} PRIVATE)
endforeach()


