#ifndef __INCLUDE_COMMON_INST_HH__
#define __INCLUDE_COMMON_INST_HH__

#include "common.hh"

namespace sim {

struct Instruction final {
  RegId rs1{};
  RegId rs2{};
  RegId rd{};

  OpType type{OpType::UNKNOWN};
  RegVal imm{};
};

} // namespace sim

#endif // __INCLUDE_COMMON_INST_HH__
