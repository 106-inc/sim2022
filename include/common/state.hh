#ifndef __INCLUDE_STATE_STATE_HH__
#define __INCLUDE_STATE_STATE_HH__

#include <array>
#include <stdexcept>

#include "common/common.hh"
#include "memory/memory.hh"

namespace sim {

class RegFile final {
private:
  std::array<RegVal, kRegNum> regs{};

public:
  Word get(RegId regnum) const { return regs.at(regnum); }

  void set(RegId regnum, Word val) {
    if (regnum == 0)
      throw std::runtime_error{"Trying to set value to register x0"};
    regs.at(regnum) = val;
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
