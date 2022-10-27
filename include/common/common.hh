#ifndef __INCLUDE_COMMON_COMMON_HH__
#define __INCLUDE_COMMON_COMMON_HH__

#include <cstdint>

namespace sim {

using Word = std::uint32_t;
using Reg = Word;
using AddrType = std::uint32_t;

constexpr Word kDummyWord = 0;

} // namespace sim

#endif // __INCLUDE_COMMON_COMMON_HH__
