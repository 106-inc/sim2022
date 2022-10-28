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


#include "enum.gen.hh"

} // namespace sim

#endif // __INCLUDE_COMMON_COMMON_HH__
