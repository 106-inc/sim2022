cmake_minimum_required(VERSION 3.15)
project(SIM2022)

include(cmake/list_dirs.cmake)
include(cmake/comp_flags.cmake)
include(cmake/clang_format.cmake)

find_package(spdlog REQUIRED)
message("Found package: spdlog")

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(thirdparty)
