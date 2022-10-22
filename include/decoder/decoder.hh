#ifndef __INCLUDE_DECODER_DECODER_HH__
#define __INCLUDE_DECODER_DECODER_HH__

#include "common/common.hh"
#include "common/inst.hh"
#include <type_traits>

namespace sim {

class Decoder final {
public:
  static Instruction decode(Word binInst);

  template <typename T> static consteval std::size_t sizeofBits() {
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

  template <std::size_t pos, bool toSet> static Word setBit(Word word) {
    static_assert(pos < sizeofBits<Word>(), "Bit index out of range");

    constexpr auto mask = Word(1) << pos;
    if constexpr (toSet)
      return word | mask;

    return word & ~mask;
  }

  template <std::size_t newSize, std::size_t oldSize>
  static Word signExtend(Word word) {
    static_assert(newSize > oldSize);
    static_assert(oldSize > 0);
    static_assert(newSize <= sizeofBits<Word>());

    using SignedWord = std::make_signed_t<Word>;

    SignedWord zeroed = getBits<oldSize - 1, 0>(word);
    struct {
      SignedWord clamped : oldSize = 0;
    } clamper{};
    clamper.clamped = zeroed;
    zeroed = clamper.clamped;

    return getBits<newSize - 1, 0>(zeroed);
  }

  template <std::size_t oldSize> static Word signExtend(Word word) {
    return signExtend<sizeofBits<Word>(), oldSize>(word);
  }
};

} // namespace sim

#endif // __INCLUDE_DECODER_DECODER_HH__
