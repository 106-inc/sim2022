add_format_exec(elfloader_test elfloader.cc)
target_link_libraries(elfloader_test PRIVATE elfloader)

upd_tar_list(elfloader_test TESTLIST)
