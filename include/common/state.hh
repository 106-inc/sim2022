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
    if (!regnum) {
       // NOP instruction looks like ADD x0, x0, 0 - assignment to x0,
       // furthermore JALR supports x0 as a destination register (to store return address if it is not needed).
       return;
    }
    regs.at(regnum) = val;
  }
};

struct State final {
  Addr pc{};
  Addr npc{};
  RegFile regs{};
  Memory mem{};
  bool branchIsTaken{false};
  bool complete{false};
};

} // namespace sim

#endif // __INCLUDE_STATE_STATE_HH__
