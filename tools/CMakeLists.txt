list_dirs(DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set(TARGETS ${DIRS})

foreach(DIR ${DIRS})
  add_subdirectory(${DIR})
  message("Included subdirectory: ${DIR}")
endforeach()

foreach(TARGET ${TARGETS})
  target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_link_libraries(${TARGET} PRIVATE spdlog::spdlog)
  set_target_properties(${TARGET}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
  )
endforeach()
