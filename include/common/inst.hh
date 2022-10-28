#ifndef __INCLUDE_COMMON_INST_HH__
#define __INCLUDE_COMMON_INST_HH__

#include "common.hh"

namespace sim {

struct Instruction final {
  RegId rs1{};
  RegId rs2{};
  RegId rs3{};

  RegId rd{};
  RegId rm{};  // rounding mode (for future use w/ floating-point operations)
  RegId csr{}; // a placeholder

  OpType type{OpType::UNKNOWN};
  RegVal imm{};
};

} // namespace sim

#endif // __INCLUDE_COMMON_INST_HH__
