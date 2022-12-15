#include <ios>
#include <sstream>
#include <string>

#include "elfloader/elfloader.hh"
#include "test_header.hh"

using namespace sim;

TEST(elfloader, ctorFail) {
  // Assign
  std::istringstream ss{"Bad input stream"};

  // Act & Assert
  EXPECT_THROW(ELFLoader{ss}, std::runtime_error);
}

TEST(elfloader, segments) {
  // Assign
  /*
    .option nopic
    .attribute arch, "rv32i2p0"
    .attribute unaligned_access, 0
    .attribute stack_align, 16

    .section .rodata
    msg:
        .string "Hello World\n"

    .section .data
    num:
        .word 97

    .text
    .align  2
    .globl  _start
    .type  _start, @function

    ######################################################
    _start:

      add x30, x30, x30
      jal LABEL1  # jump to target and save position to ra

    LABEL2:
      ecall

    LABEL1:
      jal LABEL2

    ######################################################

    .size  _start, .-_start
  */
  auto str = std::string(
      {0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x00,
       0x94, 0x00, 0x01, 0x00, 0x34, 0x00, 0x00, 0x00, 0xbc, 0x02, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x20, 0x00, 0x03, 0x00, 0x28, 0x00,
       0x08, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x70, 0xb5, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
       0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
       0x00, 0x00, 0x01, 0x00, 0xb1, 0x00, 0x00, 0x00, 0xb1, 0x00, 0x00, 0x00,
       0x05, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
       0xb1, 0x00, 0x00, 0x00, 0xb1, 0x10, 0x01, 0x00, 0xb1, 0x10, 0x01, 0x00,
       0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
       0x00, 0x10, 0x00, 0x00, 0x33, 0x0f, 0xef, 0x01, 0xef, 0x00, 0x80, 0x00,
       0x73, 0x00, 0x00, 0x00, 0xef, 0xf0, 0xdf, 0xff, 0x48, 0x65, 0x6c, 0x6c,
       0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x0a, 0x00, 0x61, 0x00, 0x00,
       0x00, 0x41, 0x1b, 0x00, 0x00, 0x00, 0x72, 0x69, 0x73, 0x63, 0x76, 0x00,
       0x01, 0x11, 0x00, 0x00, 0x00, 0x04, 0x10, 0x05, 0x72, 0x76, 0x33, 0x32,
       0x69, 0x32, 0x70, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x94, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0x00, 0x01, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
       0xb1, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x03, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xf1, 0xff, 0x08, 0x00, 0x00, 0x00,
       0xa4, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
       0x0c, 0x00, 0x00, 0x00, 0xb1, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x00, 0x94, 0x00, 0x01, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x13, 0x00, 0x00, 0x00,
       0xa0, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
       0x1a, 0x00, 0x00, 0x00, 0x9c, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x01, 0x00, 0x21, 0x00, 0x00, 0x00, 0xb1, 0x18, 0x01, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xf1, 0xff, 0x33, 0x00, 0x00, 0x00,
       0xb5, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00,
       0x54, 0x00, 0x00, 0x00, 0x94, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00,
       0x12, 0x00, 0x01, 0x00, 0x43, 0x00, 0x00, 0x00, 0xb8, 0x10, 0x01, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x4f, 0x00, 0x00, 0x00,
       0xb5, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00,
       0x5b, 0x00, 0x00, 0x00, 0xb1, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x10, 0x00, 0x03, 0x00, 0x6a, 0x00, 0x00, 0x00, 0xb5, 0x10, 0x01, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x71, 0x00, 0x00, 0x00,
       0xb8, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00,
       0x00, 0x74, 0x6d, 0x70, 0x31, 0x2e, 0x6f, 0x00, 0x6d, 0x73, 0x67, 0x00,
       0x6e, 0x75, 0x6d, 0x00, 0x24, 0x78, 0x00, 0x4c, 0x41, 0x42, 0x45, 0x4c,
       0x31, 0x00, 0x4c, 0x41, 0x42, 0x45, 0x4c, 0x32, 0x00, 0x5f, 0x5f, 0x67,
       0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x5f, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x65,
       0x72, 0x24, 0x00, 0x5f, 0x5f, 0x53, 0x44, 0x41, 0x54, 0x41, 0x5f, 0x42,
       0x45, 0x47, 0x49, 0x4e, 0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x42, 0x53, 0x53,
       0x5f, 0x45, 0x4e, 0x44, 0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x62, 0x73, 0x73,
       0x5f, 0x73, 0x74, 0x61, 0x72, 0x74, 0x00, 0x5f, 0x5f, 0x44, 0x41, 0x54,
       0x41, 0x5f, 0x42, 0x45, 0x47, 0x49, 0x4e, 0x5f, 0x5f, 0x00, 0x5f, 0x65,
       0x64, 0x61, 0x74, 0x61, 0x00, 0x5f, 0x65, 0x6e, 0x64, 0x00, 0x00, 0x2e,
       0x73, 0x79, 0x6d, 0x74, 0x61, 0x62, 0x00, 0x2e, 0x73, 0x74, 0x72, 0x74,
       0x61, 0x62, 0x00, 0x2e, 0x73, 0x68, 0x73, 0x74, 0x72, 0x74, 0x61, 0x62,
       0x00, 0x2e, 0x74, 0x65, 0x78, 0x74, 0x00, 0x2e, 0x72, 0x6f, 0x64, 0x61,
       0x74, 0x61, 0x00, 0x2e, 0x64, 0x61, 0x74, 0x61, 0x00, 0x2e, 0x72, 0x69,
       0x73, 0x63, 0x76, 0x2e, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74,
       0x65, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00,
       0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x94, 0x00, 0x01, 0x00,
       0x94, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x21, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
       0xa4, 0x00, 0x01, 0x00, 0xa4, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
       0x03, 0x00, 0x00, 0x00, 0xb1, 0x10, 0x01, 0x00, 0xb1, 0x00, 0x00, 0x00,
       0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00,
       0x03, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0xb5, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0xd4, 0x00, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00,
       0x06, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
       0x10, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00,
       0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,
       0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x7a, 0x02, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  std::istringstream ss(str);
  ELFLoader loader{ss};

  // Act
  auto loadable = loader.getLoadableSegments();
  decltype(loadable) answ{1, 2};

  // Assert
  EXPECT_EQ(loadable, answ);
}

#include "test_footer.hh"
