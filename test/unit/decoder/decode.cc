#include "test_header.hh"
#include "decoder/decoder.hh"

TEST(decoder, unknown)
{
  // Arrange
  sim::Word raw = 0;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::UNKNOWN);
  EXPECT_EQ(inst.csr, 0);
  EXPECT_EQ(inst.imm, 0);
  EXPECT_EQ(inst.rd, 0);
  EXPECT_EQ(inst.rm, 0);
  EXPECT_EQ(inst.rs1, 0);
  EXPECT_EQ(inst.rs2, 0);
  EXPECT_EQ(inst.rs3, 0);
}

TEST(decoder, lui)
{
  // Arrange
  sim::Word raw = 0b10000000000000000001'10001'0110111;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::LUI);
  EXPECT_EQ(inst.rd, 0x11);
  EXPECT_EQ(inst.imm, 0b10000000000000000001000000000000);
}

TEST(decoder, jal)
{
  // Arrange
  sim::Word raw = 0b1'1111111111'0'10000000'00011'1101111;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::JAL);
  EXPECT_EQ(inst.rd, 3);
  EXPECT_EQ(inst.imm, 0b11111111111110000000011111111110);
}

TEST(decoder, jalr)
{
  // Arrange
  sim::Word raw = 0b011110111110'10000'000'00100'1100111;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::JALR);
  EXPECT_EQ(inst.rd, 4);
  EXPECT_EQ(inst.rs1, 16);
  EXPECT_EQ(inst.imm, 0b011110111110);
}

TEST(decoder, bge)
{
  // Arrange
  sim::Word raw = 0b0110101'11110'10110'101'10011'1100011;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::BGE);
  EXPECT_EQ(inst.rs1, 0x16);
  EXPECT_EQ(inst.rs2, 0x1E);
  EXPECT_EQ(inst.imm, 0b111010110010);
}

TEST(decoder, sw)
{
  // Arrange
  sim::Word raw = 0b1000000'11111'01000'010'10001'0100011;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::SW);
  EXPECT_EQ(inst.rs1, 010);
  EXPECT_EQ(inst.rs2, 0x1F);
  EXPECT_EQ(inst.imm, 0b11111111111111111111100000010001);
}

TEST(decoder, srai)
{
  // Arrange
  sim::Word raw = 0b0100000'10011'01101'101'11000'0010011;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::SRAI);
  EXPECT_EQ(inst.rd, 0x18);
  EXPECT_EQ(inst.rs1, 13);
  EXPECT_EQ(inst.imm, 0xFFFFFFF3);
}

TEST(decoder, fence)
{
  // Arrange
  sim::Word raw = 0b010010011001'11111'000'11001'0001111;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::FENCE);
  EXPECT_EQ(inst.rd, 0x19);
  EXPECT_EQ(inst.rs1, 0x1F);
  EXPECT_EQ(inst.imm, 0b10010011001);
}

TEST(decoder, csrrsi)
{
  // Arrange
  sim::Word raw = 0b110110011001'11110'110'10101'1110011;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::CSRRSI);
  EXPECT_EQ(inst.rd, 0x15);
  EXPECT_EQ(inst.csr, 0b110110011001);
  EXPECT_EQ(inst.imm, 0xFFFFFFFE);
}

TEST(decoder, lr_w)
{
  // Arrange
  sim::Word raw = 0b10'10'00000'00010'010'10001'0101111;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::LR_W);
  EXPECT_EQ(inst.rd, 0x11);
  EXPECT_EQ(inst.rs1, 2);
  EXPECT_EQ(inst.imm, 0xFFFFFFFE);
}

TEST(decoder, fnmsub_s)
{
  // Arrange
  sim::Word raw = 0b11110'00'01110'00111'011'10101'1001011;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::FNMSUB_S);
  EXPECT_EQ(inst.rd, 0x15);
  EXPECT_EQ(inst.rm, 3);
  EXPECT_EQ(inst.rs1, 7);
  EXPECT_EQ(inst.rs2, 14);
  EXPECT_EQ(inst.rs3, 0x1E);
}

TEST(decoder, xor)
{
  // Arrange
  sim::Word raw = 0b0000000'01111'00101'100'10000'0110011;
  // Act
  auto inst = sim::Decoder::decode(raw);
  // Assert
  EXPECT_EQ(inst.type, sim::OpType::XOR);
  EXPECT_EQ(inst.rd, 0x10);
  EXPECT_EQ(inst.rs1, 5);
  EXPECT_EQ(inst.rs2, 0xF);
}


#include "test_footer.hh"
