#ifndef __INCLUDE_COMMON_INST_HH__
#define __INCLUDE_COMMON_INST_HH__

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <spdlog/fmt/fmt.h>

#include "common.hh"

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
};

using BasicBlock = std::vector<Instruction>;

} // namespace sim

template <> struct fmt::formatter<sim::Instruction> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.end(); }

  template <typename FormatContext>
  auto format(const sim::Instruction &inst, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "{}", inst.str());
  }
};

#endif // __INCLUDE_COMMON_INST_HH__
