#ifndef __INCLUDE_COMMON_COMMON_HH__
#define __INCLUDE_COMMON_COMMON_HH__

#include <bit>
#include <concepts>
#include <cstdint>

static_assert(std::endian::little == std::endian::native,
              "It seems that u r trying to run our sim on ur router");

static_assert(
    -1 == ~0,
    "Two's complement representation is required. It is fixed since c++20");

namespace sim {

using Word = std::uint32_t;
using RegVal = Word;
using Addr = std::uint32_t;
using RegId = std::size_t;

constexpr RegId kRegNum = 32;
constexpr std::uint8_t kBitsInByte = 8;
constexpr Word kDummyWord = 0;
constexpr std::uint8_t kXLENInBytes = sizeof(Word);

template <std::unsigned_integral T> constexpr auto signCast(T val) {
  return static_cast<std::make_signed_t<T>>(val);
}

template <std::unsigned_integral T> constexpr auto signAdd(T lhs, T rhs) {
  return static_cast<T>(signCast(lhs) + signCast(rhs));
}

/**
 * @brief Calculate size of a type in bits function
 * @note All calculations are guaranteed to be compile-time
 * @tparam T type to calculate its size
 * @return std::size_t size in bits
 */
template <typename T> consteval std::size_t sizeofBits() {
  return sizeof(T) * kBitsInByte;
}

/**
 * @brief Get bits from number function
 *
 * @tparam high end of bit range (msb)
 * @tparam low begin of bit range (lsb)
 * @param[in] word number to get bits from
 * @return Word bits from range [high, low] (shifted to the beginning)
 */
template <std::size_t high, std::size_t low> constexpr Word getBits(Word word) {
  static_assert(high >= low, "Incorrect bits range");
  static_assert(high < sizeofBits<Word>(), "Bit index out of range");

  auto mask = ~Word(0);
  if constexpr (high != sizeofBits<Word>() - 1)
    mask = ~(mask << (high + 1));

  return (word & mask) >> low;
}

/**
 * @brief Set bits in number function
 *
 * @tparam pos Position of bit to set (start from 0)
 * @tparam toSet value to set
 * @param[in] word number to set bit in
 * @return Word number with bit set
 */
template <std::size_t pos, bool toSet> constexpr Word setBit(Word word) {
  static_assert(pos < sizeofBits<Word>(), "Bit index out of range");

  constexpr auto mask = Word(1) << pos;
  if constexpr (toSet)
    return word | mask;

  return word & ~mask;
}

/**
 * @brief Sign extend number from one size to another
 * @details
 * The idea of sign extension is to get leftmost bit and broadcast it to
 * all new bits.
 * Consider number \f$ 110_2 \f$ (oldSize = \f$ 3 \f$).
 * Assume that we want to sign extend it to \f$ 7 \f$ bits. To simplify all
 * listings, also assume that sizeof(Word) == \f$ 1 \f$. The implemented
 * algorithm works as follows:
 *  -# All bits left to oldSize - 1 are zeroed:
 *    - \f$ 01110110_2 \f$ \f$ \rightarrow \f$  \f$ 00000110_2 \f$
 *  -# Mask for current signbit is generated:
 *    - mask \f$ \leftarrow \f$ \f$ 00000100_2 \f$
 *  -# Zeroed value is XORed with mask:
 *    - \f$ XOR \:\:\: \frac{00000110_2}{00000100_2} = 00000010_2 \f$
 *  -# Mask is subtracted from previous result:
 *    - \f$ 00000010_2 - 00000100_2 = 2_{10} - 4_{10} = 00000000_2 - 2_{10} =
 * 11111110_2 \f$
 *  -# All bits left to newSize - 1 are zeroed:
 *    - \f$ 11111110_2 \f$ \f$ \rightarrow \f$  \f$ 01111110_2 \f$
 *  -# Result is \f$ 01111110_2 = 126_{10}\f$
 *
 * If sign bit is zero, then operations with mask do nothing together \f$
 * \Rightarrow \f$ only zeroing has effect
 * @tparam newSize size to sign extend to
 * @tparam oldSize initial size
 * @param[in] word number to sign extend
 * @return Word sign extended number
 */
template <std::size_t newSize, std::size_t oldSize>
constexpr Word signExtend(Word word) {
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

/**
 * @brief Sign extend small number to Word
 *
 * @tparam oldSize current size
 * @param[in] word number to sign extend
 * @return Word sign extended number
 */
template <std::size_t oldSize> constexpr Word signExtend(Word word) {
  return signExtend<sizeofBits<Word>(), oldSize>(word);
}

} // namespace sim

#include "enum.gen.hh"

#endif // __INCLUDE_COMMON_COMMON_HH__
