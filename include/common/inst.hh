#ifndef __INCLUDE_COMMON_INST_HH__
#define __INCLUDE_COMMON_INST_HH__

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

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

  std::string str() const {
    std::stringstream ss{};

    ss << std::setw(9) << opTypeToString.at(type) << " {";
    ss << "rs1 = " << std::setw(2) << rs1 << ", ";
    ss << "rs2 = " << std::setw(2) << rs2 << ", ";
    ss << "rs3 = " << std::setw(2) << rs3 << ", ";

    ss << "rd = " << std::setw(2) << rd << ", ";
    ss << "rm = " << std::setw(2) << rm << ", ";
    ss << "csr = " << std::setw(2) << csr << ", ";

    ss << "imm = 0x" << std::hex << std::setfill('0')
       << std::setw(sizeof(imm) * 2) << imm << "}";

    return ss.str();
  }
};

using BasicBlock = std::vector<Instruction>;

} // namespace sim

#endif // __INCLUDE_COMMON_INST_HH__
