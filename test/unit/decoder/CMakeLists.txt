add_format_exec(bits bits.cc)
add_format_exec(decode_test decode.cc)
target_link_libraries(decode_test PRIVATE decoder)

upd_tar_list(decode_test TESTLIST)
upd_tar_list(bits TESTLIST)
