#ifndef __INCLUDE_DECODER_DECODER_HH__
#define __INCLUDE_DECODER_DECODER_HH__

#include "common/common.hh"
#include "common/inst.hh"

namespace sim {

class Decoder final {
public:
  static Instruction decode(Word binInst);

  template <typename T> static constexpr auto sizeofBits() {
    return sizeof(T) * kBitsInByte;
  }

  template <std::size_t high, std::size_t low> static Word getBits(Word word) {
    static_assert(high >= low, "Incorrect bits range");
    static_assert(high < sizeofBits<Word>(), "Bit index out of range");

    auto mask = ~Word(0);
    if constexpr (high != sizeofBits<Word>() - 1)
      mask = ~(mask << (high + 1));

    return (word & mask) >> low;
  }

  template <std::size_t newSize, std::size_t oldSize>
  static Word signExtend(Word word) {
    static_assert(newSize > oldSize);
    static_assert(oldSize > 0);
    static_assert(newSize <= sizeofBits<Word>());

    auto signBit = getBits<oldSize - 1, oldSize - 1>(word);
  }
};

} // namespace sim

#endif // __INCLUDE_DECODER_DECODER_HH__
