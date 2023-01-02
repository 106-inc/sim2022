#include "common/inst.hh"
#include "common/state.hh"

namespace sim {
std::string Instruction::str() const {
  std::stringstream ss{};

  ss << std::setw(9) << opTypeToString.at(type) << " {";
  ss << "rs1 = " << std::setw(2)
     << std::to_string(static_cast<std::uint64_t>(rs1)) << ", ";
  ss << "rs2 = " << std::setw(2)
     << std::to_string(static_cast<std::uint64_t>(rs2)) << ", ";
  ss << "rs3 = " << std::setw(2)
     << std::to_string(static_cast<std::uint64_t>(rs3)) << ", ";

  ss << "rd = " << std::setw(2)
     << std::to_string(static_cast<std::uint64_t>(rd)) << ", ";
  ss << "rm = " << std::setw(2)
     << std::to_string(static_cast<std::uint64_t>(rm)) << ", ";
  ss << "csr = " << std::setw(2)
     << std::to_string(static_cast<std::uint64_t>(csr)) << ", ";

  ss << "imm = 0x" << std::hex << std::setfill('0')
     << std::setw(sizeof(imm) * 2)
     << std::to_string(static_cast<std::uint64_t>(imm)) << "}";

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
