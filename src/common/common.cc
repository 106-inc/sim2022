#include "common/inst.hh"
#include "common/state.hh"

namespace sim {
std::string Instruction::str() const {
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

std::string RegFile::str() const {
  std::stringstream ss{};
  ss << std::setfill('0');
  constexpr std::size_t lineNum = 8;

  for (std::size_t i = 0; i < lineNum; ++i) {
    for (std::size_t j = 0; j < kRegNum / lineNum; ++j) {
      auto regIdx = j * lineNum + i;
      auto &reg = regs[regIdx];
      ss << "  [" << std::dec << std::setw(2) << regIdx << "] ";
      ss << "0x" << std::hex << std::setw(sizeof(reg) * 2) << reg;
    }
    ss << std::endl;
  }

  return ss.str();
}
} // namespace sim

#include "map.gen.ii"
