#include "test_header.hh"
#include "decoder/decoder.hh"

using namespace sim;

TEST(getBits, easy)
{
  // Assign
  Word val = 0xDEADBEEFFEEDDAAD;
  // Act
  auto daad = Decoder::getBits<7, 0>(val);
  auto feed = Decoder::getBits<15, 8>(val);
  auto beef = Decoder::getBits<23, 16>(val);
  auto dead = Decoder::getBits<31, 24>(val);
  // Assert
  EXPECT_EQ(daad, 0xDAAD);
  EXPECT_EQ(feed, 0xFEED);
  EXPECT_EQ(beef, 0xBEEF);
  EXPECT_EQ(dead, 0xDEAD);
}


#include "test_footer.hh"
