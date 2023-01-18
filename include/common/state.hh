#ifndef __INCLUDE_COMMON_STATE_HH__
#define __INCLUDE_COMMON_STATE_HH__

#include <array>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

#include "common/common.hh"
#include "common/counters.hh"
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

  void updateTimers(OpType type) {
    using CSRBindings = Counters::CSRBindings;

    auto join = [](Word lower, Word upper) -> DWord {
      auto tmp = static_cast<DWord>(upper) << (kBitsInByte * sizeof(Word));
      return tmp | lower;
    };

    auto split = [](DWord res, Word &lower, Word &upper) {
      constexpr auto separator = kBitsInByte * sizeof(Word);
      constexpr auto end = kBitsInByte * sizeof(DWord);
      lower = static_cast<Word>(getBits<separator - 1, 0, DWord>(res));
      upper = static_cast<Word>(getBits<end - 1, separator, DWord>(res));
    };

    auto tmp = join(regs[CSRBindings::instret], regs[CSRBindings::insreth]);
    tmp += 1;
    split(tmp, regs[CSRBindings::instret], regs[CSRBindings::insreth]);

    tmp = join(regs[CSRBindings::cycle], regs[CSRBindings::cycleh]);
    tmp += Counters::getThroughput(type);
    split(tmp, regs[CSRBindings::cycle], regs[CSRBindings::cycleh]);
  }
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
