add_library(elfloader elfloader.cc)
target_link_libraries(elfloader PUBLIC elfio::elfio)
target_include_directories(elfloader SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/ELFIO/)
