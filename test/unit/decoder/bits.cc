#include "test_header.hh"
#include "decoder/decoder.hh"

using namespace sim;

TEST(getBits, easy)
{
  // Assign
  Word val = 0xDEADBEEF;
  // Act
  auto beef = Decoder::getBits<15, 0>(val);
  auto dead = Decoder::getBits<31, 16>(val);
  auto one = Decoder::getBits<15, 15>(val);
  // Assert
  EXPECT_EQ(beef, 0xBEEF);
  EXPECT_EQ(dead, 0xDEAD);
  EXPECT_EQ(one, 0x1);
}

TEST(signExtend, easy)
{
  // Assign
  Word val = 0xBEEF;
  // Act
  auto sext = Decoder::signExtend<32, 16>(val);
  auto sext_le = Decoder::signExtend<21, 16>(val);
  auto zext_gr = Decoder::signExtend<32, 17>(val);
  auto zext_le = Decoder::signExtend<32, 15>(val);
  auto zext_leh = Decoder::signExtend<16, 15>(val);
  // Assert
  EXPECT_EQ(sext, 0xFFFFBEEF);
  EXPECT_EQ(sext_le, 0x1FBEEF);
  EXPECT_EQ(zext_gr, 0xBEEF);
  EXPECT_EQ(zext_le, 0x3EEF);
  EXPECT_EQ(zext_leh, 0x3EEF);
}

TEST(setBit, easy)
{
  // Assign
  Word val = 0xBEEF;
  // Act
  auto zero0 = Decoder::setBit<0, false>(val);
  auto one4 = Decoder::setBit<4, true>(val);
  auto one2 = Decoder::setBit<2, true>(val);
  auto zero4 = Decoder::setBit<4, false>(val);
  // Assert
  EXPECT_EQ(zero0, 0xBEEE);
  EXPECT_EQ(one4, 0xBEFF);
  EXPECT_EQ(one2, 0xBEEF);
  EXPECT_EQ(zero4, 0xBEEF);
}


#include "test_footer.hh"
