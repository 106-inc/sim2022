#ifndef __INCLUDE_COMMON_COMMON_HH__
#define __INCLUDE_COMMON_COMMON_HH__

#include <cstdint>

namespace sim {

using Word = std::uint32_t;
using RegVal = Word;
using Addr = std::uint32_t;
using RegId = std::size_t;

constexpr RegId kRegNum = 32;
constexpr std::uint8_t kBitsInByte = 8;

template <typename T> consteval std::size_t sizeofBits() {
  return sizeof(T) * kBitsInByte;
}

template <std::size_t high, std::size_t low> Word getBits(Word word) {
  static_assert(high >= low, "Incorrect bits range");
  static_assert(high < sizeofBits<Word>(), "Bit index out of range");

  auto mask = ~Word(0);
  if constexpr (high != sizeofBits<Word>() - 1)
    mask = ~(mask << (high + 1));

  return (word & mask) >> low;
}

template <std::size_t pos, bool toSet> Word setBit(Word word) {
  static_assert(pos < sizeofBits<Word>(), "Bit index out of range");

  constexpr auto mask = Word(1) << pos;
  if constexpr (toSet)
    return word | mask;

  return word & ~mask;
}

template <std::size_t newSize, std::size_t oldSize> Word signExtend(Word word) {
  static_assert(newSize >= oldSize, "Trying to sign extend to smaller size");
  static_assert(oldSize > 0, "Initial size must be non-zero");
  static_assert(newSize <= sizeofBits<Word>(), "newSize is out of bits range");

  if constexpr (newSize == oldSize)
    return word;

  Word zeroed = getBits<oldSize - 1, 0>(word);
  constexpr Word mask = Word(1) << (oldSize - 1);
  Word res = (zeroed ^ mask) - mask;

  return getBits<newSize - 1, 0>(res);
}

template <std::size_t oldSize> Word signExtend(Word word) {
  return signExtend<sizeofBits<Word>(), oldSize>(word);
}

#include "enum.gen.hh"

} // namespace sim

#endif // __INCLUDE_COMMON_COMMON_HH__
