set(SIM_RISCV_DIR ${CMAKE_SOURCE_DIR}/thirdparty/riscv-opcodes)

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

set(RISCV_YAML_DICT_PATH ${CMAKE_CURRENT_BINARY_DIR}/instr_dict.yaml)

add_custom_command(
  OUTPUT ${RISCV_YAML_DICT_PATH}
  COMMAND ${Python3_EXECUTABLE} ${SIM_RISCV_DIR}/parse.py rv_i rv_m rv_a
          rv_zicsr rv_f rv_d rv32_i
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${SIM_RISCV_DIR}/parse.py
  COMMENT "Generating RISC-V opcodes dictionary"
  VERBATIM)

set(DEC_GEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/decoder.gen.cc)
set(ENUM_GEN_FILE ${CMAKE_BINARY_DIR}/include/codegen/enum.gen.hh)
set(MAP_GEN_FILE ${CMAKE_BINARY_DIR}/src/common/map.gen.ii)

add_custom_command(
  OUTPUT ${DEC_GEN_FILE} ${ENUM_GEN_FILE} ${MAP_GEN_FILE}
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/decoder.py -y
          ${RISCV_YAML_DICT_PATH} -d ${DEC_GEN_FILE} -e ${ENUM_GEN_FILE} -m ${MAP_GEN_FILE}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/decoder.py ${RISCV_YAML_DICT_PATH}
  COMMENT
    "Generating enum & decoder files from RISC-V config. Command: ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/decoder.py -y
  ${RISCV_YAML_DICT_PATH} -d ${DEC_GEN_FILE} -e ${ENUM_GEN_FILE}"
  VERBATIM)

add_custom_target(
  DecoderGenerator
  DEPENDS ${ENUM_GEN_FILE} ${DEC_GEN_FILE} ${MAP_GEN_FILE}
  COMMENT "Checking if regeneration is required")

set(GEN_FILES ${DEC_GEN_FILE} ${ENUM_GEN_FILE} ${MAP_GEN_FILE})

add_library(decoder decoder.cc ${DEC_GEN_FILE})
format_sources(decoder "" "${GEN_FILES}" "decoder_gen" DecoderGenerator)
