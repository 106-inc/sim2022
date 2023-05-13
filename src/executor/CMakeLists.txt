add_library(executor executor.cc)
target_include_directories(executor PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/LLVMPractice/)
target_link_libraries(executor PRIVATE GraphLib)
