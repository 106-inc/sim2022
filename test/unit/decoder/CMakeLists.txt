add_executable(bits bits.cc)
add_executable(decode_test decode.cc)
target_link_libraries(decode_test PRIVATE decoder)

upd_tar_list(decode_test TESTLIST)
upd_tar_list(bits TESTLIST)
