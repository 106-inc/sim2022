#ifndef __INCLUDE_COMMON_STATE_HH__
#define __INCLUDE_COMMON_STATE_HH__

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

#include "common/common.hh"
#include "memory/memory.hh"

namespace sim {

class RegFile final {
private:
  std::array<RegVal, kRegNum> regs{};

public:
  RegVal get(RegId regnum) const { return regs.at(regnum); }

  void set(RegId regnum, RegVal val) {
    // NOP instruction looks like ADD x0, x0, 0 - assignment to x0,
    // furthermore JALR supports x0 as a destination register
    // (to store return address if it is not needed).
    if (!regnum)
      return;

    cosimLog("x{}=0x{:08x}", regnum, val);
    regs.at(regnum) = val;
  }

  std::string str() const;
};

class CSRegFile final {
private:
  std::vector<RegVal> regs{};

public:
  CSRegFile() : regs(kCSRegNum) {}

  RegVal get(CSRegId regnum) const { return regs.at(regnum); }

  void set(CSRegId regnum, RegVal val) { regs.at(regnum) = val; }
};

struct State final {
  Addr pc{};
  Addr npc{};
  RegFile regs{};
  CSRegFile csregs{};
  Memory mem{};
  bool branchIsTaken{false};
  bool complete{false};
};

} // namespace sim

#endif // __INCLUDE_COMMON_STATE_HH__
