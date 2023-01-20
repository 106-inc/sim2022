#ifndef __INCLUDE_COMMON_INST_HH__
#define __INCLUDE_COMMON_INST_HH__

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "common.hh"
#include "state.hh"

namespace sim {

struct Instruction final {
  RegId rs1{};
  RegId rs2{};
  RegId rs3{};

  RegId rd{};
  RegId rm{}; // rounding mode (for future use w/ floating-point operations)
  CSRegId csr{};

  OpType type{OpType::UNKNOWN};
  RegVal imm{};

  bool isBranch{false};

  std::string str() const;

  std::function<void(const Instruction &, State &)> callback = nullptr;
};

using BasicBlock = std::vector<Instruction>;

} // namespace sim

#endif // __INCLUDE_COMMON_INST_HH__
