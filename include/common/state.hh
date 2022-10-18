#ifndef __INCLUDE_STATE_STATE_HH__
#define __INCLUDE_STATE_STATE_HH__

#include <array>

#include "common/common.hh"
#include "memory/memory.hh"

namespace sim {

class RegFile final {
private:
  std::array<RegVal, kRegNum> regs{};

public:
  Word get(RegId regnum) const { return regs.at(regnum); }

  void set(RegId regnum, Word val) {
    regs.at(regnum) = regnum == 0 ? 0 : val;
  }
};

struct State final {
  Addr pc{};
  Addr npc{};
  RegFile regs{};
  Memory mem{};
};

} // namespace sim

#endif // __INCLUDE_STATE_STATE_HH__
