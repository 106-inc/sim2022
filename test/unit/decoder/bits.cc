#include "decoder/decoder.hh"
#include "test_header.hh"

TEST(sizeofBits, uint32) { ASSERT_EQ(sim::sizeofBits<std::uint32_t>(), 32); }

TEST(getBits, easy) {
  // Assign
  sim::Word val = 0xDEADBEEF;
  // Act
  auto beef = sim::getBits<15, 0>(val);
  auto dead = sim::getBits<31, 16>(val);
  auto one = sim::getBits<15, 15>(val);
  // Assert
  EXPECT_EQ(beef, 0xBEEF);
  EXPECT_EQ(dead, 0xDEAD);
  EXPECT_EQ(one, 0x1);
}

TEST(signExtend, easy) {
  // Assign
  sim::Word val = 0xBEEF;
  // Act
  auto sext = sim::signExtend<32, 16>(val);
  auto sext_le = sim::signExtend<21, 16>(val);
  auto zext_gr = sim::signExtend<32, 17>(val);
  auto zext_le = sim::signExtend<32, 15>(val);
  auto zext_leh = sim::signExtend<16, 15>(val);
  // Assert
  EXPECT_EQ(sext, 0xFFFFBEEF);
  EXPECT_EQ(sext_le, 0x1FBEEF);
  EXPECT_EQ(zext_gr, 0xBEEF);
  EXPECT_EQ(zext_le, 0x3EEF);
  EXPECT_EQ(zext_leh, 0x3EEF);
}

TEST(signExtend, full) {
  // Assign
  sim::Word val = 0xBEEF;
  // Act
  auto sext = sim::signExtend<16>(val);
  auto zext_gr = sim::signExtend<17>(val);
  auto zext_le = sim::signExtend<15>(val);
  // Assert
  EXPECT_EQ(sext, 0xFFFFBEEF);
  EXPECT_EQ(zext_gr, 0xBEEF);
  EXPECT_EQ(zext_le, 0x3EEF);
}

TEST(setBit, easy) {
  // Assign
  sim::Word val = 0xBEEF;
  // Act
  auto zero0 = sim::setBit<0, false>(val);
  auto one4 = sim::setBit<4, true>(val);
  auto one2 = sim::setBit<2, true>(val);
  auto zero4 = sim::setBit<4, false>(val);
  // Assert
  EXPECT_EQ(zero0, 0xBEEE);
  EXPECT_EQ(one4, 0xBEFF);
  EXPECT_EQ(one2, 0xBEEF);
  EXPECT_EQ(zero4, 0xBEEF);
}

#include "test_footer.hh"
