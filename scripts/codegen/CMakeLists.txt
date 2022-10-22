find_package(
  Python3
  COMPONENTS Interpreter
  REQUIRED)

set(SIM_RISCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/riscv-opcodes/)

execute_process(
  COMMAND ${Python3_EXECUTABLE} -m pip show pyyaml
  RESULT_VARIABLE EXIT_CODE
  OUTPUT_QUIET)

if(NOT ${EXIT_CODE} EQUAL 0)
  message(
    FATAL_ERROR
      "The \"PyYaml\" Python3 package is not installed. Please install it using the following command: \"pip3 install pyyaml\"."
  )
endif()


add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/decode_generated.cc
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/decoder.py ${CMAKE_CURRENT_BINARY_DIR}
COMMAND ${CMAKE_COMMAND} -E copy ${SIM_RISCV_DIR}/constants.py ${CMAKE_CURRENT_BINARY_DIR}
COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/decoder.py
VERBATIM)
