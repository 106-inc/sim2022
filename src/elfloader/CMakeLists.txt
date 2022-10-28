add_library(elfloader elfloader.cc)
target_link_libraries(elfloader PRIVATE elfio::elfio)
target_include_directories(elfloader SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/ELFIO/)
