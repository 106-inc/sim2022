add_executable(elfdump main.cc)
format_target(elfdump ${CMAKE_CURRENT_SOURCE_DIR} main.cc)

target_link_libraries(elfdump PRIVATE elfio::elfio)

