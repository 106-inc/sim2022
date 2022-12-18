#ifndef __INCLUDE_STATE_STATE_HH__
#define __INCLUDE_STATE_STATE_HH__

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "common/common.hh"
#include "memory/memory.hh"

namespace sim {

class RegFile final {
private:
  std::array<RegVal, kRegNum> regs{};

public:
  Word get(RegId regnum) const { return regs.at(regnum); }

  void set(RegId regnum, Word val) {
    // NOP instruction looks like ADD x0, x0, 0 - assignment to x0,
    // furthermore JALR supports x0 as a destination register
    // (to store return address if it is not needed).
    if (!regnum)
      return;

    regs.at(regnum) = val;
  }

  std::string str() const {
    std::stringstream ss{};
    ss << std::setfill('0');
    constexpr std::size_t colNum = 4;

    for (std::size_t i = 0; i < kRegNum / colNum; ++i) {
      for (std::size_t j = 0; j < colNum; ++j) {
        auto regIdx = j * colNum + i;
        auto &reg = regs[regIdx];
        ss << "  [" << std::dec << std::setw(2) << regIdx << "] ";
        ss << "0x" << std::hex << std::setw(sizeof(reg) * 2) << reg;
      }
      ss << std::endl;
    }

    return ss.str();
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
