#ifndef __INCLUDE_STATE_STATE_HH__
#define __INCLUDE_STATE_STATE_HH__

#include <array>

#include "common/common.hh"
#include "memory/memory.hh"

namespace sim {

class RegFile final {
private:
  std::array<Reg, 32> regs{};

public:
  Word get(std::size_t regnum) const { return regs.at(regnum); }

  void set(std::size_t regnum, Word val) {
    regs.at(regnum) = regnum == 0 ? 0 : val;
  }
};

struct State final {
  Reg pc{};
  Reg npc{};
  RegFile regs{};
  Memory mem{};
};

} // namespace sim

#endif // __INCLUDE_STATE_STATE_HH__
