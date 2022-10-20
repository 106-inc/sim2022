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
  // Assert
  EXPECT_EQ(beef, 0xBEEF);
  EXPECT_EQ(dead, 0xDEAD);
}


#include "test_footer.hh"
